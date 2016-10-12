
CFLAGS += -Wall
obj := cantest
src := cantest.c
CC  := arm-linux-gcc

$(obj): $(src)
	$(CC) $(CFLAGS) $^ -o $@ -g

.PHONY: clean
clean:
	-rm cantest
