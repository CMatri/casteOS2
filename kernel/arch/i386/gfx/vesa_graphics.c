#include <string.h>
#include <kernel/kernel.h>
#include <kernel/heap.h>
#include <kernel/ports.h>
#include <kernel/logger.h>
#include <kernel/screen.h>
#include <kernel/paging.h>
#include <kernel/vesa_graphics.h>
#include <kernel/nasa_logo.h>

uint32_t mouse_pos_x;
uint32_t mouse_pos_y;
uint8_t* buffer;
uint32_t buffer_size;
int VBE_HD;
struct wind_list **wm_handles;
bitmap_img_t* kickit;

uint64_t wm_num_windows;
struct wind_list* root_window;
struct wind_list* ch;

uint16_t mouse_bmp[19] = {
	0b0110000000000000,
	0b0111000000000000,
	0b0111100000000000,
	0b0111110000000000,
	0b0111111000000000,
	0b0111111100000000,
	0b0111111110000000,
	0b0111111111000000,
	0b0111111111100000,
	0b0111111111110000,
	0b0111111111111000,
	0b0111111111111100,
	0b0111111111111100,
	0b0111111111000000,
	0b0111101111100000,
	0b0111000111100000,
	0b0110000011110000,
	0b0000000011110000,
	0b0000000001100000,
};

void vesa_mouse_packet(mouse_device_packet_t packet) {
	//if(packet.buttons & LEFT_CLICK) klog("CLICK");
	if(mouse_pos_x + packet.x_difference < screen_width && mouse_pos_x + packet.x_difference > 0) mouse_pos_x += packet.x_difference;
	if(mouse_pos_y - packet.y_difference < screen_height && mouse_pos_y + packet.y_difference > 0) mouse_pos_y -= packet.y_difference;
}

void vesa_clear_screen(uint32_t color) {
	vesa_fill_rect(0, 0, 0, screen_width, screen_height, color);
}

void draw_mouse(int x, int y) {
	if(x < screen_width && y < screen_height) {
		uint16_t cx, cy;
		uint32_t mask[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 14, 1 << 15, 1 << 16 };

		uint8_t *where = buffer + x * pixel_width + y * bytes_per_line;

		for(cy = 0; cy < 19; cy++) {
			for(cx = 1; cx < 16; cx++) {
				where[cx * 4] = mouse_bmp[cy] & mask[16 - cx] ? 255 - where[cx * 4] : where[cx * 4];
				where[cx * 4 + 1] = mouse_bmp[cy] & mask[16 - cx] ? 255 - where[cx * 4 + 1] : where[cx * 4 + 1];
				where[cx * 4 + 2] = mouse_bmp[cy] & mask[16 - cx] ? 255 - where[cx * 4 + 2] : where[cx * 4 + 2];
			}
			where += bytes_per_line;
		}
	}
}

void vesa_fill_rect(bitmap_t* bmp, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	if(bmp) {
		int i, j;
		for(i = 0; i < width; i++) {
			for(j = 0; j < height; j++) {
				vesa_put_pixel(bmp, x + i, y + j, color);
			}
		}
	} else {
		uint8_t* where = buffer + x * pixel_width + y * bytes_per_line;
		uint32_t i, j;
		for(i = 0; i < height; i++) {
			for(j = 0; j < width; j++) {
				where[j * 4] = color & 255;
				where[j * 4 + 1] = (color >> 8) & 255;
				where[j * 4 + 2] = (color >> 16) & 255;
				where[j * 4 + 3] = (color >> 24) & 255;
			}
			where += bytes_per_line;
		}
	}
}

void vesa_put_pixel(bitmap_t* bmp, uint16_t x, uint16_t y, uint32_t color) {
	if(bmp) {
		if(x > bmp->width || y > bmp->height) return;
		uint8_t* where = bmp->data + x + y * bmp->width * pixel_width;
		where[0] = color & 255;
		where[1] = (color >> 8) & 255;
		where[2] = (color >> 16) & 255;
		where[3] = (color >> 24) & 255;	   	
	} else {
		if(x > screen_width || y > screen_height) return;
		unsigned where = x * pixel_width + y * bytes_per_line;

	   	buffer[where] = color & 255;
	   	buffer[where + 1] = (color >> 8) & 255;
	   	buffer[where + 2] = (color >> 16) & 255;
      	buffer[where + 3] = (color >> 24) & 255;
   }
}

void vesa_draw_bitmap(bitmap_t* bmp, uint32_t x, uint32_t y) {
	if(x > screen_width || y > screen_height) return;
    uint32_t ty = 0;
    uint8_t* loc = (uint8_t*)(buffer + y * bytes_per_line + x * pixel_width);
    for (; ty < bmp->height; ty++){
		memcpy((uint8_t*)(loc + ty * bytes_per_line), (uint8_t*)(bmp->data + ty * bmp->width * pixel_width), bmp->width * pixel_width);
    }
}

void vesa_draw_bitmap_image(bitmap_img_t* bmp, uint16_t x, uint16_t y) {
	uint8_t* bytes = bmp->image_bytes;
	uint8_t* where = (uint8_t*) (buffer + x * pixel_width + y * bytes_per_line);
	int i, j;
	for(j = 1; j < bmp->height; j++) {
		int cy = bmp->height - j;
		for(i = 0; i < bmp->width; i++) {
			where[i * 4] = bytes[cy * bmp->width * 4 + i * 4];
			where[i * 4 + 1] = bytes[cy * bmp->width * 4 + i * 4 + 1];
			where[i * 4 + 2] = bytes[cy * bmp->width * 4 + i * 4 + 2];
			where[i * 4 + 3] = 0x0;
		}
		where += bytes_per_line;
	}
}

void repaint_children(uint32_t parent) {
	struct wind_list *wnd, *child;
	if (parent >= wm_num_windows) return;

	wnd = wm_handles[parent];
	if (!wnd) return;
	if (wnd->needs_repaint) {
	//	windowborder(&wnd->wbmp, 0, 0, wnd->wbmp.width, wnd->wbmp.height);
		wnd->needs_repaint = 0;
	}

	vesa_draw_bitmap(&wnd->wbmp, wnd->position.x1, wnd->position.y1);
	for (child = wnd->first_child; child != 0; child = child->next) repaint_children(child->handle);
}

void move_to_front(struct wind_list* wnd) {
	if (wnd->prev != 0) wnd->prev->next = wnd->next;
    if (wnd->next != 0) wnd->next->prev = wnd->prev;
    if (wnd == wnd->parent->first_child) wnd->parent->first_child = wnd->next;
    if (wnd == wnd->parent->last_child) wnd->parent->last_child = wnd->prev;
    wnd->prev = wnd->parent->last_child;
    wnd->next = 0;
    if (wnd->parent->last_child != 0) wnd->parent->last_child->next = wnd;
    wnd->parent->last_child = wnd;
    if (wnd->parent->first_child == 0) wnd->parent->first_child = wnd;
}

struct wind_list* create_window(struct wind_list* prev_wind, char* caption, int x, int y, int width, int height) {
	if(wm_num_windows + 1 >= MAX_WINDOWS) return -1;
	struct wind_list* w = (struct wind_list*) kmalloc(sizeof(struct wind_list), 0);
	memset((uint8_t*) w, 0x0, sizeof(struct wind_list));
	w->position.x1 = x;
	w->position.y1 = y;
	w->position.x2 = x + width;
	w->position.y2 = y + height;
	w->handle = wm_num_windows;
	w->caption = caption;
	w->wbmp.width = width;
	w->wbmp.height = height;
	w->wbmp.size = pixel_width * width * height;
	w->wbmp.data = (uint32_t*) kmalloc(w->wbmp.size, 0);
	memset((uint8_t*) w->wbmp.data, 0x0, w->wbmp.size);

	if(!prev_wind) {
		w->prev = 0;
		w->next = 0;
		w->first_child = 0;
		w->last_child = 0;
		w->parent = 0;
	} else {
		w->prev = prev_wind;
		w->next = 0;
		w->first_child = 0;
		w->last_child = 0;
		w->parent = prev_wind->parent ? prev_wind->parent : prev_wind;
		move_to_front(w);
	}

	wm_handles[wm_num_windows++] = w;

	return w;
}

vbe_info_t* vesa_get_vbe_info() {
	return info;
}

void vesa_update_graphics() {
	vesa_clear_screen(0x0);
	tty_draw();	
	//repaint_children(root_window->handle);
	//vesa_draw_bitmap_image(kickit, screen_width - kickit->width, screen_height - kickit->height);
	draw_mouse(mouse_pos_x, mouse_pos_y);

	while ((port_byte_in(0x3DA) & 0x08));
    while (!(port_byte_in(0x3DA) & 0x08));
	memcpy(lfb, buffer, buffer_size);
}

void vesa_graphics_init(struct multiboot_header* mbt) {
	VBE_HD = 1;//!((int) &GFX_MODE);
	info  = (vbe_info_t*) (mbt->vbe_mode_info + BASE_VIRTUAL);
	lfb = (uint8_t*) info->physbase;
	uint32_t old_lfb = (uint32_t) lfb;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;
	pixel_width = bytes_per_line / screen_width; // bytes per pixel
	mouse_pos_x = screen_width / 2;
	mouse_pos_y = screen_height / 2;
	wm_num_windows = 0;

	buffer_size = (uint32_t) screen_width * screen_height * pixel_width;
	map_virtual_address_space(current_page_directory(), (uint32_t) lfb, (uint32_t) lfb, 1, buffer_size); // Identity map linear frame buffer
	buffer = (uint8_t*) kmalloc(buffer_size, 0);

    //wm_handles = kmalloc(sizeof(struct wind_list*) * MAX_WINDOWS, 0);
    //memset((uint8_t*) wm_handles, 0, sizeof(struct wind_list*) * MAX_WINDOWS);
    
	//root_window = create_window(0, "root window", 100, 100, 1600, 720);
	//ch = create_window(root_window, "child window", 10, 10, 40, 20);

	//fill_rect(&root_window->wbmp, 0, 0, root_window->wbmp.width, root_window->wbmp.height, 0x00FF0000);
	//fill_rect(&ch->wbmp, 0, 0, ch->wbmp.width, ch->wbmp.height, 0x0000FF00);

//	module_t* m = get_module("kickitwconnor");
//	kickit = load_bitmap(m->mod_start, m->mod_end - m->mod_start);

	memset(buffer, 0x0, buffer_size);
}