gcc -I. -I $OPENCL/inc -lOpenCL mm_host.c snucl_utils.c -o test01
ldd test01
./test01
