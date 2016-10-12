EXEC = serial-test
OBJS = serial-test.o
SRC  = serial-test.c

CC = arm-none-linux-gnueabi-gcc

#LD = arm-linux-ld
CFLAGS += -O2 -Wall
LDFLAGS += 

all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -pthread

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -pthread

clean:
	@rm -vf $(EXEC) *.o *~
