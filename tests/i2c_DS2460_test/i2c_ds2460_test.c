#include     <stdio.h>
#include     <stdlib.h> 
#include     <unistd.h>  
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h> 
#include     <termios.h>
#include     <errno.h>  

#include "iic.h"

#define I2C_ADDR  0x80
#define DATA_LEN 17

int main(void)
{
    	unsigned int uiRet;
    	int i;

    	unsigned char tx_buf[DATA_LEN];
    	unsigned char rx_buf[DATA_LEN];
    	unsigned char addr[2] ;

    	addr[0] = 0x00;
    	    
    	GiFd = open("/dev/i2c-0", O_RDWR);   
    	if(GiFd == -1)
        	perror("open serial 0\n");

  	uiRet = ioctl(GiFd, I2C_SLAVE, I2C_ADDR >> 1);
    	if (uiRet < 0) {
		printf("setenv address faile ret: %x \n", uiRet);
		return -1;
     	}

	tx_buf[0] = addr[0];
	for (i = 1; i < DATA_LEN; i++)
        		tx_buf[i] = i;

    	write(GiFd, tx_buf, DATA_LEN);
    	write(GiFd, addr, 1);
    	read(GiFd, rx_buf, DATA_LEN - 1);

    	printf("read from ds2460's eeprom:");
    	for(i = 0; i < DATA_LEN - 1; i++) {
         	printf(" %x", rx_buf[i]);
    	}

    	printf("\n");
	
	return 0;
}
