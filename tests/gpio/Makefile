EXEC = gpio_test
OBJS = gpio_test.o
SRC  = gpio_test.c

CC = arm-none-linux-gnueabi-gcc 
CFLAGS += 
LDFLAGS += 
all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@rm -vf $(EXEC) *.o *~
