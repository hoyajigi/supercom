#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <CL/cl.h>
#include "cl_util.h"
#include "timers.h"

#define ARRAY_SIZE 1048576

void reference_sort(int *array, int size);


void bitonic_sort_opencl(int *array, int size) {
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
    cl_mem           m_array;
    cl_int           err;
	cl_event         ev_bp;
    cl_uint          num_dev = 0;
	int num_stage;
	int stage,pass;	
    int i;

    // Platform
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    CHECK_ERROR(err);
    if(num_platforms == 0) {
        printf("No OpenCl platform");
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
        printf("No device");
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
    char * source_code=get_source_code("bitonic.cl");


    program = clCreateProgramWithSource(context, 1, (const char **)&source_code, NULL, &err);
    CHECK_ERROR(err);

	// Callback data for clBUildProgram
	ev_bp=clCreateUserEvent(context,&err);
	CHECK_ERROR(err);
	bp_data_t bp_data;
	bp_data.dev=dev;
	bp_data.event=&ev_bp;

    // Build the program.
    err = clBuildProgram(program, 1, &dev, NULL, build_program_callback, &bp_data);
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


    // Buffers
    
    m_array=clCreateBuffer(context, CL_MEM_READ_WRITE,
            sizeof(int) * size, NULL, &err);
    CHECK_ERROR(err);

	err=clEnqueueWriteBuffer(cmd_queue, m_array,CL_FALSE,0,size*sizeof(int),array,0,NULL,NULL);

	size_t lws[2]={64,4};
	size_t gws[2]={1024,(size/2)/1024};
	
	// Number of stages
	int temp = size;
	num_stage=0;
	while(temp>1){
		temp/=2;
		num_stage++;
	}

	clWaitForEvents(1,bp_data.event);
	
	kernel=clCreateKernel(program,"bitonic_sort",&err);
	CHECK_ERROR(err);

	clFinish(cmd_queue);
	timer_start(2);

	for(stage=0;stage<num_stage;stage++){
		for(pass=0;pass<=stage;pass++){
   			// Set the arguments.
			err=clSetKernelArg(kernel,0,sizeof(cl_mem),&m_array);
			err|=clSetKernelArg(kernel,1,sizeof(int),&stage);
			err|=clSetKernelArg(kernel,2,sizeof(int),&pass);

			// 
			err=clEnqueueNDRangeKernel(cmd_queue,kernel,2,NULL,gws,lws,0,NULL,NULL);
			CHECK_ERROR(err);
		}
	}
	
    // Read the result.
    
    err = clEnqueueReadBuffer(cmd_queue,
            m_array,
            CL_TRUE, 0,
            sizeof(int) * size,
            array,
            0, NULL, NULL);
	
	timer_stop(2);    
    CHECK_ERROR(err);
	printf("Kernel time : %lf sec\n",timer_read(2));
    //Release
	clReleaseEvent(ev_bp);
    clReleaseMemObject(m_array);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
    free(platforms);

}


int main(int argc, char** argv) {
  int *array;
  int *array_ref;
  int size = ARRAY_SIZE;
  int success;
  int i;

  timer_init();

  srand(time(NULL));
  array = (int*)malloc(size * sizeof(int));
  array_ref = (int*)malloc(size * sizeof(int));
  for (i = 0; i < ARRAY_SIZE; i++) {
    array[i] = rand();
    array_ref[i] = array[i];
  }

  timer_start(0);
  bitonic_sort_opencl(array, size);
  timer_stop(0);

  timer_start(1);
  reference_sort(array_ref, size);
  timer_stop(1);

  printf("OCL bitonic sort time : %.5f\n", timer_read(0));
  printf("Sequential quick sort time : %.5f\n", timer_read(1));

  success = 1;
  for (i = 0; i < size; i++) {
    if (array[i] != array_ref[i]) {
      success = 0;
      break;
    }
  }
  if (success)
    printf("Verification SUCCESS\n");
  else
    printf("Verification FAIL\n");

  free(array);
  free(array_ref);
  return 0;
}

void quicksort(int *array, int begin, int end) {
  int leftarrow, rightarrow;
  int temp, pivot;

  leftarrow = begin;
  rightarrow = end;
  pivot = array[(begin+end)/2];
  while (1)
  {
    while (array[rightarrow] > pivot)
      rightarrow = rightarrow - 1;

    while (array[leftarrow] < pivot)
      leftarrow = leftarrow + 1;

    if (leftarrow <= rightarrow)
    {
      temp = array[leftarrow];
      array[leftarrow] = array[rightarrow];
      array[rightarrow] = temp;
      leftarrow = leftarrow + 1;
      rightarrow = rightarrow - 1;
    }

    if (rightarrow < leftarrow)
      break;
  }

  if (begin < rightarrow)
    quicksort(array, begin, rightarrow);
  if (leftarrow < end)
    quicksort(array, leftarrow, end);

  return;
}

void reference_sort(int *array, int size) {
  quicksort(array, 0, size - 1);
}
