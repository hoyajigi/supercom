/*
 * OpenCL Boilerplate by Hyunseok Cho (i@hoyajigi.com)
 * First created at 2013.10.02
 * All Rights Reserved
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<strings.h>
#include<CL/cl.h>


#define CHECK_ERROR(err) if (err != CL_SUCCESS) { fprintf(stderr, "[%s:%d] ERROR: %d\n",__FILE__,__LINE__,err);exit(EXIT_FAILURE); }

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
	context = cl_CreateContext(NULL, 1, &dev, NULL, NULL, &err);
	CHECK_ERROR(err);

	// Command queue
	cmd_queue = clCreateCommandQueue(context, dev, 0, &err);
	CHECK_ERROR(err);

	// Create a program
	char * source_code="";
	
	
	// TODO : Get source code in your favor
	
	size_t source_len=strlen(source_code);
	program = clCreateProgramWithSource(context, 1, (const char **)&source_code, &source_len, &err);
	CHECK_ERR(err);

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
		fprintf(stderr,"%s\n"log);
		fprintf(stderr,"-------------------------------\n");
		free(log);

		CHECK_ERR(err);
	}
	
	// Kernel
	kernel = clCreate


	// Buffers
	



	// Set the arguments.
	



	// Enqueue the kernel.
	




	//Release

	return EXIT_SUCCESS;
}

