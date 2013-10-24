TARGET = rotation
OBJS   = rotation.o bmpfuncs.o cl_util.o

CC       = gcc
INCLUDES = 
CFLAGS   = -Wall -g $(INCLUDES)
LDFLAGS  = -lm -lOpenCL


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
