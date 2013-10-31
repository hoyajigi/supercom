#include <stdio.h>
#include <stdlib.h>
#include "libjpeg_wrapper.h"

struct RGB* open_jpeg_image(struct jpeg_decompress_struct* dinfo, const char* filename)
{
	struct jpeg_error_mgr jerr;
	dinfo->err = jpeg_std_error(&jerr);
	jpeg_create_decompress(dinfo);

	FILE* fp = fopen(filename, "rb");
	if( fp == NULL )
	{
		printf("File open error. filename=%s\n", filename);
		return 0;
	}

	jpeg_stdio_src(dinfo, fp);

	jpeg_read_header(dinfo, TRUE);

	jpeg_start_decompress(dinfo);

	int rowstride = dinfo->output_width * dinfo->output_components;
	int x;
	int y =0;
	JSAMPARRAY buffer = (*dinfo->mem->alloc_sarray)((j_common_ptr)dinfo, JPOOL_IMAGE, rowstride, 1);

	struct RGB* rgb = (struct RGB*)malloc(dinfo->output_height*dinfo->output_width*sizeof(struct RGB));

	while( dinfo->output_scanline < dinfo->output_height )
	{
		jpeg_read_scanlines(dinfo, buffer, 1);
		for( x = 0; x < dinfo->output_width; x++ )
		{
			JSAMPLE* p = buffer[0] + 3 * x;
			rgb[y*dinfo->output_width+x].r = p[0];
			rgb[y*dinfo->output_width+x].g = p[1];
			rgb[y*dinfo->output_width+x].b = p[2];
		}
		y++;
	}

	jpeg_finish_decompress(dinfo);
	jpeg_destroy_decompress(dinfo);
	fclose(fp);

	return rgb;
}

void save_jpeg_image(const char* filename, struct RGB* rgb, int image_width, int image_height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer;
	int i;
	FILE* fp;

	cinfo.err = jpeg_std_error(&jerr);

	fp = fopen(filename, "wb");
	if( fp == NULL )
	{
		printf("Cannot open file to save jpeg image: %s\n", filename);
		exit(0);
	}
	
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);

	jpeg_start_compress(&cinfo, TRUE);

	for(i = 0; i < image_height; i++ )
	{
		row_pointer = (JSAMPROW)&rgb[i*image_width];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(fp);
}
