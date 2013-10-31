#include<CL/cl.h>
#include<snucl_utils.h>

int main(int argc, char** argv)
{
	cl_platform_id platform;
	cl_device_type dev_type=CL_DEVICE_TYPE_GPU;
	cl_device_id device;
	cl_context context;
	cl_command_queue cmq;
	cl_program program;
	cl_kernel kernel;
	cl_mem memA,memB,memC;

	float *A,*B,*c;
	int wA,hA,wB,hB,wC,hC;
	size_t sizeA,sizeB,sizeC;
	size_t global[2],local[2];
	int i,j;

	// Step 1. 행렬 초기화
	
	// Step 2. OpenCL 오브젝트 초기화
	
	// Step 3. 커널 생성 및 빌드
	
	// Step 4. 버퍼 쓰기
	
	// Step 5. 커널 시작
	
	// Step 6. 버퍼 일기 및 결과값 확인


	return 0;
}
