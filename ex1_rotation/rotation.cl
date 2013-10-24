__kernel void img_rotate(__global float* dest_data,
		__global float* src_data,
		int W,
		int H,
		float sinTheta,
		float cosTheta) {
	// Work-item gets its index within index space
	const int ix= get_global_id(0);
	const int iy=get_global_id(1);

	// Calculate location of data to move into (ix,iy)
	// Output decompostion as mentioned
	float x0=W/2.0f;
	float y0=H/2.0f;

	float xOff=ix-x0;
	float yOff=iy-y0;

	int xpos=(int)(xOff*cosTheta+yOff*sinTheta+x0);
	int ypos=(int)(yOff*cosTheta-xOff*sinTheta+y0);

	// Bounds Checking
	if((xpos>=0)&&(xpos<W)&&(ypos>=0)&&(ypos<H)) {
		dest_data[iy*W+ix]=src_data[ypos*W+xpos];
	} else
		dest_data[iy*W+ix]=0.0f;
}
