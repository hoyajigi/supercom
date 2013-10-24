/*
 * OpenCL Boilerplate by Hyunseok Cho (i@hoyajigi.com)
 * First created at 2013.10.04
 * All Rights Reserved
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<strings.h>
#include<CL/cl.h>
#include"bmpfuncs.h"
#include"cl_util.h"


int main(int argc,char **argv)
{
	/* Host data structures */
	cl_platform_id   *platforms;
	cl_uint          num_platforms;
	cl_device_type   dev_type = CL_DEVICE_TYPE_GPU;
	cl_device_id     dev;
	cl_context       context;
	// NOTE : You might have multiple cmd_queue but whatever
	cl_command_queue cmd_queue;
	cl_program       program;
	cl_kernel        kernel;
	// TODO : define your variables
	cl_mem           d_input,d_output;
	cl_int           err;
	cl_uint          num_dev = 0;
	int i;

	// Set the image rotation (in radians)
	float theta=M_PI/6;
	float cos_theta=cosf(theta);
	float sin_theta=sinf(theta);
//	printf("theta = %f ");
	
	// Rows and columns in the input image
	int imageHeight;
	int imageWidth;

	const char* inputFile="input.bmp";
	const char* outputFile="output.bmp";

	// Homegrown function to read a BMP from file
	float* inputImage=readImage(inputFile,&imageWidth,&imageHeight);
	
	int dataSize=imageHeight*imageWidth*sizeof(float);

	//Output image on the host
	float* outputImage=NULL;
	outputImage=(float *)malloc(dataSize);
	
	/*
	char *dtype=getenv("CL_DEV_TYPE");
	if(dtype)
	*/

	// Platform
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	CHECK_ERROR(err);
	if(num_platforms == 0) {
//		ERROR("No OpenCl platform");
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
//		ERROR("No device");
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

	d_input=clCreateBuffer(context,CL_MEM_READ_ONLY,dataSize,NULL,&err);
	CHECK_ERROR(err);

	d_output=clCreateBuffer(context,CL_MEM_WRITE_ONLY,dataSize,NULL,&err);
	CHECK_ERROR(err);

	err=clEnqueueWriteBuffer(cmd_queue,d_input,CL_FALSE,0,dataSize,inputImage,0,NULL,NULL);
	CHECK_ERROR(err);

	// Create a program
	char * source_code=get_source_code("./rotation.cl");
	
	// TODO : Get source code in your favor
	
	size_t source_len=strlen(source_code);
	program = clCreateProgramWithSource(context, 1, (const char **)&source_code, NULL, &err);
	CHECK_ERROR(err);
	free(source_code);

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
	kernel = clCreateKernel(program,"img_rotate",&err);
	CHECK_ERROR(err);

	// Buffers
	// TODO: make and buffers
	/*
	clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
			sizeof(float) * N, A, &err);
	*/
	CHECK_ERROR(err);

	// Set the arguments.
	err=clSetKernelArg(kernel,0,sizeof(cl_mem),&d_output);
	err=clSetKernelArg(kernel,1,sizeof(cl_mem),&d_input);
	err=clSetKernelArg(kernel,2,sizeof(int),&imageWidth);
	err=clSetKernelArg(kernel,3,sizeof(int),&imageHeight);
	err=clSetKernelArg(kernel,4,sizeof(float),&sin_theta);
	err=clSetKernelArg(kernel,5,sizeof(float),&cos_theta);
	CHECK_ERROR(err);


	// Enqueue the kernel.
	size_t gws[2]={imageWidth,imageHeight};
	err=clEnqueueNDRangeKernel(cmd_queue,kernel,2,NULL,gws,NULL,0,NULL,NULL);
	CHECK_ERROR(err);

	// Read the result.
	
	err = clEnqueueReadBuffer(cmd_queue,
			d_output,
			CL_TRUE, 0,
			dataSize,
			outputImage,
			0, NULL, NULL);
	
	CHECK_ERROR(err);

	storeImage(outputImage,outputFile,imageHeight,imageWidth,inputFile);

	//Release
	clReleaseMemObject(d_input);
	clReleaseMemObject(d_output);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmd_queue);
	clReleaseContext(context);

	return EXIT_SUCCESS;
}

