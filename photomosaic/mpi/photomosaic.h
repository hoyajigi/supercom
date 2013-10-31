#ifndef __PHOTOMOSAIC_H__
#define __PHOTOMOSAIC_H__

struct image_file
{
	char filename[100];
	struct jpeg_decompress_struct dinfo;
	struct RGB* rgb;
};

void create_photomosaic(struct image_file* target_image,
		struct RGB* photomosaic_rgb, struct image_file* tile_images, int num_images);
#endif //__PHOTOMOSAIC_H__
