EXEC    = wdt_test
OBJS    = wdt_test.o

CROSS   = arm-fsl-linux-gnueabi-
CC      = $(CROSS)gcc
STRIP   = $(CROSS)strip
CFLAGS  = -Wall -g -O2

.PHONY:all
all:    clean $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	$(STRIP) $@
	rm -f $(EXEC)*.o

.PHONY:clean
clean:
	rm -f $(EXEC)*.o

