#include <kernel/initrd.h>
#include <kernel/heap.h>
#include <string.h>

initrd_header_t *initrd_header;
initrd_file_header_t *file_headers;
fs_node_t *initrd_root;            
fs_node_t *initrd_dev;             
fs_node_t *root_nodes;             
int nroot_nodes;

struct dirent dir_entry;

static uint32_t initrd_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
	initrd_file_header_t header = file_headers[node->inode];
	if (offset > header.length)
		return 0;
	if (offset + size > header.length)
		size = header.length-offset;
	memcpy(buffer, (uint8_t*) (header.offset + offset), size);
	return size;
}

static struct dirent* initrd_readdir(fs_node_t *node, uint32_t index) {
	if (node == initrd_root && index == 0) {
		strcpy(dir_entry.name, "dev");
		dir_entry.name[3] = 0;
		dir_entry.ino = 0;
		return &dir_entry;
	}

	if (index - 1 >= nroot_nodes)	return 0;

	strcpy(dir_entry.name, root_nodes[index - 1].name);
	dir_entry.name[strlen(root_nodes[index - 1].name)] = 0;
	dir_entry.ino = root_nodes[index - 1].inode;
	return &dir_entry;
}

static fs_node_t* initrd_finddir(fs_node_t *node, char *name) {
	if (node == initrd_root && !strcmp(name, "dev")) return initrd_dev;

	int i;
	for (i = 0; i < nroot_nodes; i++)
		if (!strcmp(name, root_nodes[i].name))
			return &root_nodes[i];
   return 0;
}

fs_node_t* initrd_init(uint32_t location) {
	initrd_header = (initrd_header_t*) location;
	file_headers = (initrd_file_header_t*) (location + sizeof(initrd_header_t));

	initrd_root = (fs_node_t*) kmalloc(sizeof(fs_node_t), 0);
	strcpy(initrd_root->name, "initrd");
	initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
	initrd_root->flags = FS_DIRECTORY;
	initrd_root->read = 0;
	initrd_root->write = 0;
	initrd_root->open = 0;
	initrd_root->close = 0;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = 0;
	initrd_root->impl = 0;

	initrd_dev = (fs_node_t*) kmalloc(sizeof(fs_node_t), 0);
	strcpy(initrd_dev->name, "dev");
	initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
	initrd_dev->flags = FS_DIRECTORY;
	initrd_dev->read = 0;
	initrd_dev->write = 0;
	initrd_dev->open = 0;
	initrd_dev->close = 0;
	initrd_dev->readdir = &initrd_readdir;
	initrd_dev->finddir = &initrd_finddir;
	initrd_dev->ptr = 0;
	initrd_dev->impl = 0;

	root_nodes = (fs_node_t*) kmalloc(sizeof(fs_node_t) * initrd_header->nfiles, 0);
   	nroot_nodes = initrd_header->nfiles;
	
	int i;
	for (i = 0; i < initrd_header->nfiles; i++) {
		file_headers[i].offset += location;
		strcpy(root_nodes[i].name, &file_headers[i].name);
		root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
		root_nodes[i].length = file_headers[i].length;
		root_nodes[i].inode = i;
		root_nodes[i].flags = FS_FILE;
		root_nodes[i].read = &initrd_read;
		root_nodes[i].write = 0;
		root_nodes[i].readdir = 0;
		root_nodes[i].finddir = 0;
		root_nodes[i].open = 0;
		root_nodes[i].close = 0;
		root_nodes[i].impl = 0;
	}

	return initrd_root;
}

void print_initrd() {
	int i = 0;
	struct dirent *node = 0;
	kprint("Reading initrd:\n");
	while((node = readdir_fs(fs_root, i)) != 0) {
		kprint("  Found file ");
		kprint(node->name);
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

		if((fsnode->flags & 0x7) == FS_DIRECTORY) kprint("\n    (directory)\n");
		else {
			kprint("\n    contents: \"");
			char buf[256];
			uint32_t sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++) kprint_char(buf[j]);
			kprint("\"\n");
		}

		i++;
	}
}