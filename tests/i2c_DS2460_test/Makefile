
CFLAGS += 
obj := i2c_ds2460_test
src := main.c
CC  := arm-fsl-linux-gnueabi-gcc

$(obj): $(src)
	$(CC) $(CFLAGS)  -o $@  $^

.PHONY: clean
clean:
	-rm i2c_ds2460_test
