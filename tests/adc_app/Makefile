EXEC = lradc_test
OBJS = lradc_test.o
SRC  = lradc_test.c

CC = arm-fsl-linux-gnueabi-gcc 
CFLAGS += 
LDFLAGS += 
all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@rm -vf $(EXEC) *.o *~
