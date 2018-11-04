#include <string.h>
#include <string.h>
#include <kernel/kernel.h>
#include <kernel/heap.h>
#include <kernel/logger.h>
#include <kernel/screen.h>
#include <kernel/paging.h>
#include <kernel/vesa_graphics.h>

uint8_t* buffer;
uint32_t buffer_size;
int VBE_HD;

void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	uint8_t* loc = buffer + x * pixel_width + y * bytes_per_line;
	int i, j;
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			loc[j * 4] = color & 255;
			loc[j * 4 + 1] = (color >> 8) & 255;
			loc[j * 4 + 2] = (color >> 16) & 255;
		}

		loc += bytes_per_line;
	}
}

void put_pixel(uint16_t x, uint16_t y, uint32_t color) {
	if(x > screen_width || y > screen_height) return;

	unsigned where = x * pixel_width + y * bytes_per_line;

   	buffer[where] = color & 255;
   	buffer[where + 1] = (color >> 8) & 255;
   	buffer[where + 2] = (color >> 16) & 255;
}

void draw_char(uint8_t c, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background) {
	uint16_t cx, cy;
	uint16_t mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	for(cy = 0; cy < 12; cy++) {
		for(cx = 0; cx < 8; cx++) {
			put_pixel(x + cx, y + cy, Terminess_Powerline_Bold[c][cy] & mask[7 - cx] ? foreground : background);
		}
	}
}

void draw_string(char* string, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background) {
	int i = 0;
	for(; i < strlen(string); i++) draw_char(string[i], x + (i * 8), y, foreground, background);
}

void update_graphics() {
	int x = 0;
	for(; x < 160; x++) fill_rect(x, 30, 10, 50, (x << 24) | (x << 16) | ((x*10) & 0x0000FF));
	draw_string("CasteOS2 kernel initialized.", 10, 10, 0xFFFFFF, 0x0);
	memcpy(lfb, buffer, buffer_size);
}

void graphics_init(struct multiboot_header* mbt) {
	VBE_HD = 1;//!((int) &GFX_MODE);
	info  = (vbe_info_t*) (mbt->vbe_mode_info + BASE_VIRTUAL);
	lfb = (uint8_t*) info->physbase;
	uint32_t old_lfb = (uint32_t) lfb;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;
	pixel_width = bytes_per_line / screen_width;

	asm volatile("bpointdebug:");
	
	buffer_size = (uint32_t) screen_width * screen_height * pixel_width;
	map_virtual_address_space(current_page_directory(), (uint32_t) lfb, (uint32_t) lfb, buffer_size);
	buffer = (uint8_t*) kmalloc(buffer_size);

	klog("Graphics buffer: 0x");
	klhex((uint32_t) buffer);
	klog("\nLinear frame buffer location: 0x");
	klhex((uint32_t) lfb);
	klog(" (VIRT), 0x");
	klhex(old_lfb);
	klog(" (PHYS)\nBytes per line: ");
	kldec(bytes_per_line);
	klog("\nWidth: ");
	kldec(screen_width);
	klog("\nHeight: ");
	kldec(screen_height);
	klog("\n");

	memset(buffer, 0x00, buffer_size);
}