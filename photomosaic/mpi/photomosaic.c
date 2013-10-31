#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "libjpeg_wrapper.h"
#include "photomosaic.h"

// Calculate the differences between the target image and the tile image.
// The difference is calculated from the fomula: sqrt((r-r')^2 + (g-g')^2 + (b-b')^2);
// where a pixel of the target image and the tile image is represented as 
// (r,g,b) and (r', g', b'), respectively.
// This function returns the sum of pixel differences.
float calc_diff(struct image_file* target_image, struct image_file* tile_image,
		int target_start_x, int target_start_y, 
		int img_idx)
{
	int x, y;
	float diff = 0.0;
	int tile_width = tile_image->dinfo.output_width;
	int tile_height = tile_image->dinfo.output_height;
	int image_width = target_image->dinfo.output_width;
	int image_height = target_image->dinfo.output_height;
	int num_tiles_x = image_width / tile_width;
	int num_tiles_y = image_height / tile_height;

	for( y = 0; y < tile_height; y++ )
	{
		for( x = 0; x < tile_width; x++ )
		{
			struct RGB* target_rgb = &target_image->rgb[(target_start_y*tile_height+y) * image_width
				+ (target_start_x*tile_width+x)];
			float diff_r = ((float)target_rgb[0].r) - tile_image->rgb[y*tile_width+x].r;
			float diff_g = ((float)target_rgb[0].g) - tile_image->rgb[y*tile_width+x].g;
			float diff_b = ((float)target_rgb[0].b) - tile_image->rgb[y*tile_width+x].b;

			diff += sqrtf(diff_r*diff_r + diff_g*diff_g + diff_b*diff_b);
		}
	}

	return diff;
}

struct image_file* find_the_most_similar_image(struct image_file* target_image, int start_x,
		int start_y, struct image_file* tile_images, int num_images, int* pmin_img_idx, float* pmin_diff)
{
	int img_idx;
	int min_img_idx = -1;
	float min_diff = FLT_MAX;

	for( img_idx = 0; img_idx < num_images; img_idx++)
	{
		float diff = calc_diff(target_image, &tile_images[img_idx], start_x, start_y, img_idx);

		if( min_diff > diff )
		{
			min_diff = diff;
			min_img_idx = img_idx;
		}
	}

	*pmin_img_idx = min_img_idx;
	*pmin_diff = min_diff;
	return &tile_images[min_img_idx];
}

void copy_tile(struct RGB* photomosaic_rgb, int image_width, int start_x, int start_y,
		struct image_file* tile_image)
{
	int tile_width = tile_image->dinfo.output_width;
	int tile_height = tile_image->dinfo.output_height;
	int y;

	for( y = 0; y < tile_height; ++y )
	{
		memcpy( (void*)&photomosaic_rgb[(start_y*tile_height+y)*image_width+(start_x*tile_width)], 
				&tile_image->rgb[y*tile_width], 
				tile_width*sizeof(struct RGB));
	}
}

void create_photomosaic(struct image_file* target_image,
		struct RGB* photomosaic_rgb, struct image_file* tile_images, int num_images)
{
	int tile_width = tile_images[0].dinfo.output_width;
	int tile_height = tile_images[0].dinfo.output_height;
	int image_width = target_image->dinfo.output_width;
	int image_height = target_image->dinfo.output_height;

	if( image_width % tile_width != 0 ||
			image_height % tile_height != 0 )
	{
		printf("The resolution of target image(%d,%d) is not divided by tile images(%d,%d)!\n",
				target_image->dinfo.output_width, target_image->dinfo.output_height,
				tile_width, tile_height);
		exit(0);
	}

	int num_tiles_x = image_width / tile_width;
	int num_tiles_y = image_height / tile_height;
	int x, y;

	int min_img_idx;
	float min_diff;

	for( y = 0; y < num_tiles_y; ++y )
	{
		for( x = 0; x < num_tiles_x; ++x )
		{
			struct image_file* min_image = find_the_most_similar_image(target_image, 
					x, y, tile_images, num_images, &min_img_idx, &min_diff);

			copy_tile(photomosaic_rgb, image_width,
					x, y, min_image);
		}
	}

}
