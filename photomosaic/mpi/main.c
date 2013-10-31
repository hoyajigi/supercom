#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "libjpeg_wrapper.h"
#include "photomosaic.h"
#include "timers.h"

////////////////////////////////////////////////////////////////////////////////
void printUsage(char* prog_name)
{
	printf("Usage: %s <target_image> <tile_image_list>\n", prog_name);
}

////////////////////////////////////////////////////////////////////////////////
// load image list for tiles.
struct image_file* load_image_list(const char* filename, int* p_num_images)
{
	int num_images;
	int i;

	FILE* fp = fopen(filename, "r");
	if( fp == NULL )
	{
		printf("image list file open failed. filename=%s\n", filename);
		exit(0);
	}
	fscanf(fp, "%d\n", &num_images);

	struct image_file* images = (struct image_file*)malloc(sizeof(struct image_file)*num_images);

	for( i = 0; i < num_images; ++i )
	{
		fscanf(fp, "%s\n", images[i].filename );
		images[i].rgb = open_jpeg_image(&images[i].dinfo, images[i].filename);
	}

	if( p_num_images ) *p_num_images = num_images;

	return images;
}

////////////////////////////////////////////////////////////////////////////////
// Transform filename a.jpg to a_mosaic.jpg.
void add_mosaic_into_filename(char* output_filename, const char* orig_filename)
{
	char* dst = output_filename;
	const char* endp = orig_filename + strlen(orig_filename);
	const char* startp = endp;
	const char* src = orig_filename;
	while( *--endp != '.' );
	while( *(startp-1) != '/' && startp != orig_filename)
		startp--;

	while( startp < endp )
	{
		*dst++ = *startp++;
	}
	strcpy(dst, "_mosaic.jpg");
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if( argc != 3 )
	{
		printUsage(argv[0]);
		return 0;
	}

	char* target_image_filename = argv[1];
	char* image_list_filename = argv[2];
	char output_filename[1000];
	struct image_file target_image;
	struct image_file target_image_per_node;

	int num_images;
	int original_height;
	int image_width;
	int image_height_per_node;
	int rank,size;
	int i;

	MPI_Init(&argc,&argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	printf("MPI! %d of %d\n",rank,size);

	// Read the target image.
	timer_start(1);
	printf("[%d] Opening target image..\n",rank);
	strcpy(target_image.filename, target_image_filename);
	target_image.rgb = open_jpeg_image(&target_image.dinfo, target_image.filename);
	timer_stop(1);

	image_width=target_image.dinfo.output_width;
	original_height=target_image.dinfo.output_height;
	image_height_per_node=original_height/size;

	// make a local image.
	target_image_per_node.dinfo=target_image.dinfo;
	target_image_per_node.dinfo.output_height=image_height_per_node;
	target_image_per_node.rgb=target_image.rgb+(image_width*image_height_per_node)*rank;

	struct RGB* photomosaic_rgb = (struct RGB*)malloc(image_width * 
			original_height*sizeof(struct RGB));
	printf("[%d] Kernel Time elapsed: %f sec\n",rank, timer_read(1));
	printf("[%d] Target image size(original) = (%d,%d)\n",rank, target_image.dinfo.output_width,
			target_image.dinfo.output_height);
	printf("[%d] Target image size(pernode) = (%d,%d)\n",rank,target_image_per_node.dinfo.output_width,target_image_per_node.dinfo.output_height);

	// Read the tile images.
	timer_clear(1);
	timer_start(1);
	printf("[%d] Reading tile images..\n",rank);
	struct image_file* tile_images = load_image_list(image_list_filename, &num_images);
	timer_stop(1);
	printf("[%d] Time elapsed: %f sec\n",rank, timer_read(1));

	// Create photomosaic!
	timer_clear(1);
	timer_start(1);
	printf("Creating a photomosaic image..\n");
	create_photomosaic_opencl(&target_image, photomosaic_rgb, tile_images, num_images);
	timer_stop(1);
	printf("[%d] Time elapsed: %f sec\n",rank, timer_read(1));

	if(rank!=0){
		MPI_Send(photomosaic_rgb,
				image_width*image_height_per_node*sizeof(struct RGB),
				MPI_BYTE,0,123,MPI_COMM_WORLD);
	}
	else{
		MPI_Status status;
		for(i=1;i<size;++i){
			int err=MPI_Recv(photomosaic_rgb+(image_width*image_height_per_node)*i,
					image_width*image_height_per_node*sizeof(struct RGB),
					MPI_BYTE,i,123,MPI_COMM_WORLD,&status);
			if(err!=MPI_SUCCESS){
				printf("[%d] MPI_Recv failure. error = %d, status = %d\n",rank,err,status);
				MPI_Finalize();
				exit(0);
			}
		}
	}
	// Save the result.
	timer_clear(1);
	timer_start(1);
	printf("Saving the photomosaic image..\n");
	add_mosaic_into_filename(output_filename, target_image_filename);
	save_jpeg_image(output_filename, photomosaic_rgb, 
			target_image.dinfo.output_width, target_image.dinfo.output_height);
	timer_stop(1);
	printf("[%d] Time elapsed: %f sec\n",rank, timer_read(1));
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
	free(photomosaic_rgb);

	return 0;
}
