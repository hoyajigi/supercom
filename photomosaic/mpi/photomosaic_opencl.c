#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <CL/cl.h>
#include "cl_util.h"
#include "timers.h"
#include "libjpeg_wrapper.h"
#include "photomosaic.h"

#define MAX_DEVS 8

cl_event events_for_calc_min[MAX_DEVS][2];

// Calculate the differences between the target image and the tile image.
// The difference is calculated from the fomula: sqrt((r-r')^2 + (g-g')^2 + (b-b')^2);
// where a pixel of the target image and the tile image is represented as 
// (r,g,b) and (r', g', b'), respectively.
// This function returns the sum of pixel differences.
float calc_diff(cl_context context,
		cl_program program,
		cl_command_queue cmd_queues[MAX_DEVS],
		struct image_file* target_image, 
		struct image_file* tile_images,
		int num_images,
		cl_mem mem_diff_sum[MAX_DEVS],
		int num_devs)
{
	int x, y;
	int tile_width = tile_images[0]->dinfo.output_width;
	int tile_height = tile_image[0]->dinfo.output_height;
	int image_width = target_image->dinfo.output_width;
	int image_height = target_image->dinfo.output_height;
	int num_tiles_x = image_width / tile_width;
	int num_tiles_y = image_height / tile_height;
	cl_int err;
	int i;
	int dev;

	// Kernel
	cl_kernel kernel_calc_diff[MAX_DEVS];
	cl_kernel kernel_sum_diff[MAX_DEVS];
	cl_event waiting_events[MAX_DEVS][4];

	cl_mem mem_target_image[MAX_DEVS];
	cl_mem mem_tile_image[MAX_DEVS];
	cl_mem mem_diff[MAX_DEVS];

	for(dev=0;dev<num_devs;++dev){
		kernel_calc_diff[dev]=clCreateKernel(program,"calc_diff",&err);
		CHECK_ERROR(err);
		kernel_sum_diff[dev]=clCreateKernel(program,"sum_diff",&err);
		CHECK_ERROR(err);

		// Buffers
		mem_target_image[dev]=clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(unsigned char)*3*tile_width*tile_height,NULL,&err);
		CHECK_ERROR(err);
		mem_tile_image[dev]=clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(unsigned char)*3*tile_width*tile_height,NULL,&err);
		CHECK_ERROR(err);
		mem_diff[dev]=clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(float)*image_width*image_height_per_dev,NULL,&err);
		CHECK_ERROR(err);

		// Set the arguments for kernel_calc_diff
		err=clSetKernelArg(kernel_calc_diff[dev],0,sizeof(cl_mem),&mem_target_image[dev]);
		CHECK_ERROR(err);
		err=clSetKernelArg(kernel_calc_diff[dev]);
	}

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


#define ERROR(err) fprintf(stderr, "[%s:%d] ERROR: %s\n",__FILE__,__LINE__,err);exit(EXIT_FAILURE);


int main()
{
	/* Host data structures */
	cl_platform_id   *platforms;
	cl_uint          num_platforms;
	cl_device_type   dev_type = CL_DEVICE_TYPE_DEFAULT;
	cl_device_id     dev;
	cl_context       context;
	// NOTE : You might have multiple cmd_queue but whatever
	cl_command_queue cmd_queue;
	cl_program       program;
	cl_kernel        kernel;
	// TODO : define your variables
//	cl_mem           ;
	cl_int           err;
	cl_uint          num_dev = 0;
	
	int i;

	// Platform
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	CHECK_ERROR(err);
	if(num_platforms == 0) {
		ERROR("No OpenCl platform");
	}
	printf("Number of platforms: %u\n",num_platforms);
	platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
	err = clGetPlatformIDs(num_platforms,platforms,NULL);
	CHECK_ERROR(err);

	//Device
	for(i=0;i<num_platforms;i++) {
		// FIXME : something wrong
		err = clGetDeviceIDs(platforms[i],dev_type,1,&dev,&num_dev);
		if(err != CL_DEVICE_NOT_FOUND) CHECK_ERROR(err);
		if(num_dev == 1) break;
	}
	if(num_dev<1) {
		ERROR("No device");
	}
	
	// Print the device name.
	size_t name_size;
	clGetDeviceInfo(dev, CL_DEVICE_NAME, 0, NULL, &name_size);
	char *dev_name = (char *)malloc(name_size + 1);
	err = clGetDeviceInfo(dev,CL_DEVICE_NAME,name_size,dev_name,NULL);
	CHECK_ERROR(err);
	printf("Device: %s\n",dev_name);
	free(dev_name);

	// Context
	context = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
	CHECK_ERROR(err);

	// Command queue
	cmd_queue = clCreateCommandQueue(context, dev, 0, &err);
	CHECK_ERROR(err);

	// Create a program
	char * source_code="";
	
	// TODO : Get source code in your favor
	
	size_t source_len=strlen(source_code);
	program = clCreateProgramWithSource(context, 1, (const char **)&source_code, &source_len, &err);
	CHECK_ERROR(err);

	// Build the program.
	err = clBuildProgram(program, 1, &dev, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		// Print the build log.
		size_t log_size;
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
				0, NULL, &log_size);
		char *log = (char *)malloc(log_size + 1);
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
				log_size, log, NULL);
		fprintf(stderr,"\n");
		fprintf(stderr,"---------- BUILD LOG ----------\n");
		fprintf(stderr,"%s\n",log);
		fprintf(stderr,"-------------------------------\n");
		free(log);

		CHECK_ERROR(err);
	}
	
	// Kernel
	//kernel = clCreateKernel(program,"",&err);
	CHECK_ERROR(err);

	// Buffers
	// TODO: make and buffers
	/*
	clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
			sizeof(float) * N, A, &err);
	*/
	CHECK_ERROR(err);

	// Set the arguments.
	

	// Enqueue the kernel.
	//err=clEnqueueNDRangeKernel(cmd_queue,kernel,1,NULL,gws,lws,0,NULL,NULL);
	CHECK_ERROR(err);

	// Read the result.
	/*
	err = clEnqueueReadBuffer(cmd_queue,
			mem_C,
			CL_TRUE, 0,
			sizeof(float) * N,
			C,
			0, NULL, NULL);
	*/
	CHECK_ERROR(err);

	//Release
	//clReleaseMemObject();
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmd_queue);
	clReleaseContext(context);
	free(platforms);

	return EXIT_SUCCESS;
}

