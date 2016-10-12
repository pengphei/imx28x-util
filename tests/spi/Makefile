
CFLAGS += -Wall
obj := spidev_test
src := spidev_test.c
CC = arm-none-linux-gnueabi-gcc 

$(obj): $(src)
	$(CC) $(CFLAGS) $^ -o $@ -g

.PHONY: clean
clean:
	-rm spidev_test
