__kernel void bitonic_sort(__global int* array, int stage,int pass)
{
	int gid=get_global_id(0)+get_global_id(1)*get_global_size(0);
	int distance=1<<(stage-pass);
	int block_width=2*distance;
	int bitonic_width=1<<stage;
	int left=(gid%distance)+(gid/distance)*block_width;
	int right=left+distance;
	int increasing=((gid/bitonic_width)%2==0);

	int left_value=array[left];
	int right_value=array[right];
	if(increasing){
		array[left]=min(left_value,right_value);
		array[right]=max(left_value,right_value);
	}
	else{
		array[left]=max(left_value,right_value);
		array[right]=min(left_value,right_value);
	}
}
