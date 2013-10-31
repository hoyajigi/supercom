#ifndef __LIBJPEG_WRAPPER_H__
#define __LIBJPEG_WRAPPER_H__

#include <jpeglib.h>

struct RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct RGB* open_jpeg_image(struct jpeg_decompress_struct* dinfo, const char* filename);
void save_jpeg_image(const char* filename, struct RGB* rgb, int image_width, int image_height);

#endif // __LIBJPEG_WRAPPER_H__

