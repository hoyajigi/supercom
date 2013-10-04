TARGET   = opencl_boilerplate
OBJS     = opencl_boilerplate.o

CC       = gcc
INCLUDES = 
CFLAGS   = -O4 -Wall $(INCLUDES)
LDFLAGS  = -lm -lOpenCL

all: $(TARGET)

$(TARGET): $(OBJS)
			 $(CC) $(LDFLAGS) $(OBJS) -o $@
.c.o:
	 $(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
