#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>
#include     <termios.h>
#include     <errno.h>

#define DEV_PATH        "/sys/devices/virtual/gpio/gpio68/value"

int main(void)
{
        int fd = 0;
        char value;

        fd = open(DEV_PATH, O_RDWR);
        if (fd < 0) {
                perror(DEV_PATH);
        }

        lseek(fd, 0, SEEK_SET);
        read(fd, &value, 1);
        printf("get value:%c \n", value);

        close(fd);
        return 0;
}
