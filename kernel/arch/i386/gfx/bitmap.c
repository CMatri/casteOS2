#include <kernel/bitmap.h>
#include <kernel/logger.h>
#include <kernel/heap.h>

bitmap_img_t* load_bitmap(uint8_t* buf, uint32_t total_size) {
    bitmap_img_t* ret = kmalloc(sizeof(bitmap_img_t), 0); 
    bmp_fileheader_t* header = (bmp_fileheader_t*) (buf);
    bmp_infoheader_t* info = (bmp_infoheader_t*) (buf + sizeof(bmp_fileheader_t));
    
    ret->width = info->biWidth;
    ret->height = info->biHeight;
    ret->image_bytes = (void*)((uint32_t) buf + (uint32_t) header->bfOffBits);
    ret->buf = buf;
    ret->total_size = total_size;
    ret->bpp = info->biBitCount;
    return ret;
}