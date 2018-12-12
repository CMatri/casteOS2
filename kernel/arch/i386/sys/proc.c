#include <kernel/proc.h>
#include <kernel/logger.h>
#include <kernel/kernel.h>
#include <kernel/heap.h>
#include <kernel/paging.h>
#include <string.h>

process_t* pqueue;
process_t* cur_proc;
int cur_pid = 0;

void idle_thread() {
	klog("I'm an idle thread lol");
}

void test_thread0() {
	klog("test");
}

volatile void switch_task(registers_t *regs) {
 	if(!pqueue || !cur_proc) return;
 	klog("switching task");

 	memcpy(&cur_proc->regs, regs, sizeof(registers_t));
 
	if (cur_proc->next != 0) cur_proc = cur_proc->next;
	else cur_proc = pqueue;
	
	memcpy(regs, &cur_proc->regs, sizeof(registers_t));
	load_page_dir(virtual2phys(kpage_dir, cur_proc->page_dir));

	switch_kernel_task(cur_proc->regs.eip, cur_proc->regs.esp);
}

void tasking_init() {
	create_process(&idle_thread, PAGE_SIZE);
	//create_process(test_thread0, PAGE_SIZE);
}

void create_process(uint8_t* code, uint32_t code_length) {
	process_t* proc = (process_t*) kmalloc(sizeof(process_t), 0);
	proc->pid = cur_pid++;
	proc->page_dir = (page_directory_t*) kmalloc(sizeof(page_directory_t), 1);

	process_t* p = pqueue;
	if(!p) {
		pqueue = proc;
		cur_proc = proc;
	} else {
		while(p->next) p = p->next;
		p->next = proc;
	}

	memset(proc->page_dir, 0, sizeof(page_directory_t));
	copy_page_directory(proc->page_dir, kpage_dir);
	map_virtual_address(proc->page_dir, 0, -1, 1); 							// .text
	map_virtual_address(proc->page_dir, PAGE_ALIGN(code_length), -1, 1); 	// stack
	load_page_dir(virtual2phys(kpage_dir, proc->page_dir));
	memcpy(0x0, code, code_length);
	
	//load_page_dir(virtual2phys(kpage_dir, kpage_dir));
	//switch_to_user_mode(0x0, PAGE_ALIGN(code_length));
}
