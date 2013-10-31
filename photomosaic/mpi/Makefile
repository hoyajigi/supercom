TARGET   = photomosaic
OBJS     = libjpeg_wrapper.o main.o photomosaic_opencl.o timer.o cl_util.o

CC       = mpicc
INCLUDES = #-I/home/sc56/src/AMD_Samples/include/
CFLAGS   = -std=c99 -O4 -Wall $(INCLUDES)
LDFLAGS  = -lm -lOpenCL -ljpeg#-L/opt/AMDAPP/lib/x86_64

all: $(TARGET)

$(TARGET): $(OBJS)
			 $(CC) $(OBJS) -o $@ $(LDFLAGS)
.c.o:
	 $(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
