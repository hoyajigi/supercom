TARGET = bitonic
OBJS   = bitonic.o cl_util.o timers.o

CC       = gcc
INCLUDES = 
CFLAGS   = -O4 -Wall -g $(INCLUDES)
LDFLAGS  = -lm -lOpenCL


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.stdout *.stderr
