#include <kernel/bitmap.h>
#include <kernel/logger.h>
#include <kernel/heap.h>

bitmap_img_t* load_bitmap(uint8_t* buf, uint32_t total_size) {
	bitmap_img_t* ret = kmalloc(sizeof(bitmap_img_t), 0); 
	bmp_fileheader_t* header = (bmp_fileheader_t*) (buf);
	uint32_t offset = header->bfOffBits;
    klog("bitmap size: ");
    klhex(header->bfSize);
    klog("\nbitmap offset: ");
    klhex(offset);
    klog("\n");

    bmp_infoheader_t* info = (bmp_infoheader_t*) (buf + sizeof(bmp_fileheader_t));

    ret->width = info->biWidth;
    ret->height = info->biHeight;
    ret->image_bytes = (void*)((uint32_t) buf + offset);
    ret->buf = buf;
    ret->total_size = total_size;
    ret->bpp = info->biBitCount;
    klog("bitmap is ");
    klhex(ret->width);
    klog(" x ");
    klhex(ret->height);
    klog(" x ");
    kldec(ret->bpp);
    klog("\n");
    return ret;
}