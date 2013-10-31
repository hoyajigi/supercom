__kernel void calc_diff(__global uchar* target_image,
		__global uchar* tile_image,
		__global float* diff)
{
	int global_x=get_global_id(0);
	int global_y=get_global_id(1);
	int tile_x=global_x%TILE_WIDTH;
	int tile_y=global_y%TILE_HEIGHT;

	float3 target_image_pixel=(float3)(
			target_image[(global_y*IMAGE_WIDTH+global_x)*3],
			target_image[(global_y*IMAGE_WIDTH+global_x)*3+1],
			target_image[(global_y*IMAGE_WIDTH+global_x)*3+2]);
	float3 tile_image_pixel=(float3)(
			tile_image[(tile_y*TILE_WIDTH+tile_x)*3],
			tile_image[(tile_y*TILE_WIDTH+tile_x)*3+1],
			tile_image[(tile_y*TILE_WIDTH+tile_x)*3+2]);
	diff[global_y*IMAGE_WIDTH+global_x]=distance(target_image_pixel,tile_image_pixel);
}

// work_group_size should be (TILE_WIDTH,1).
__kernel void sum_diff(__global float* diff,
		__global float* diff_sum,
		int image_idx)
{
	int image_x=get_global_id(0);
	int tile_y=get_global_id(1);
	int tile_x=image_x/TILE_WIDTH;
	int x_in_tile=image_x%TILE_WIDTH;
	int i;
	__local float diff_sub_sum[TILE_WIDTH];

	diff_sub_sum[x_in_tile]=0.0f;
	for(i=0;i<TILE_HEIGHT;++i){
		diff_sub_sum[x_in_tile]+=diff[(tile_y*TILE_HEIGHT+i)*IMAGE_WIDTH+image_x];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	if(x_in_tile==0)
	{
		float sum=0.0f;
		for(i=0;i<TILE_WIDTH;++i){
			sum+=diff_sub_sum[i];
		}
		diff_sum[(NUM_TILES_X*NUM_TILES_Y)*image_idx+tileY*NUM_TILES_X+tile_x]=sum;
	}
}

__kernel void calc_min(__global float* diff_sum,
		__global int* min_img_idx)
{
	int global_x=get_global_id(0);
	int global_y=get_global_id(1);
	int i;
	int min_idx=0;
	float min_diff=diff_sum[0*(NUM_TILES_Y*NUM_TILES_X)+global_y*NUM_TILES_X+global_x];
	for(i=1;i<NUM_IMAGES;++i){
		float cur_diff=diff_sum[i*(NUM_TILES_Y*NUM_TILES_X)+global_y*NUM_TILES_X+global_x];
		if(min_diff>cur_diff){
			min_diff=cur_diff;
			min_idx=i;
		}
	}
	min_img_idx[global_y*NUM_TILES_X+global_x]=min_idx;
}

