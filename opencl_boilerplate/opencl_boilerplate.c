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

#define CHECK_ERROR(err) if (err != CL_SUCCESS) { fprintf(stderr, "[%s:%d] ERROR: %d\n",__FILE__,__LINE__,err);exit(EXIT_FAILURE); }
#define ERROR(err) fprintf(stderr, "[%s:%d] ERROR: %s\n",__FILE__,__LINE__,err);exit(EXIT_FAILURE);

char *get_source_code(const char *filename,size_t *len);

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

char *get_source_code(const char *file_name,size_t *len)
{
	FILE *file=fopen(file_name,"r");
	if(file==NULL){
		ERROR("Failed to open source code");
	}

	fseek(file, 0, SEEK_END);
	size_t length = (size_t)ftell(file);
	rewind(file);
	
	char *source_code=(char *)malloc(length+1);
	if(fread(source_code,length,1,file)!=1){
		fclose(file);
		free(source_code);

		ERROR("Failed to read source code");
	}
	fclose(file);
	
	*len=length;
	source_code[length]='\0';
	return source_code;
}
