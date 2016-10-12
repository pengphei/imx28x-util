/*********************************************************************************
**	ADC参考电压为1.85V，外部输入电压不能超过3.7V,内部带有一个模拟除2电路。
**   The pad ESD protection limits the voltage on the LRADC0-LRADC6 inputs to VDDIO. The BATT and 5V inputs to the
**   LRADC have built-in dividers to handle the higher voltages.
**   cmd:10-17直接读，20-27开启硬件除2 
**        10-17对应相应通道，cmd不等于12、13、14、15，此4通道已经用作TouchScreen。    
*********************************************************************************/

#include<stdio.h>	/* using printf()        */
#include<stdlib.h>      /* using sleep()         */
#include<fcntl.h>       /* using file operation  */
#include<sys/ioctl.h>   /* using ioctl()         */

int main(int argc, char *argv[])
{
	int fd;
	int iRes;
	int time = 100;
	double val;

	fd = open("/dev/magic-adc", 0);
	if(fd < 0){
		printf("open error by APP- %d\n",fd);
		close(fd);
		return 0;
	}

	while(time--){
		sleep(1);		

		ioctl(fd, 20, &iRes);			/* 开启除2     CH0    */
		val = (iRes * 3.7) / 4096.0;
		printf("CH0:%.2f\t", val);

		ioctl(fd, 11, &iRes);                   /* 不开除2     CH1    */
		val = (iRes * 1.85) / 4096.0;
       		printf("CH1:%.2f\t", val);

        	ioctl(fd, 26, &iRes);                   /* 开启除2     CH6    */
		val = (iRes * 3.7) / 4096.0;
        	printf("CH6:%.2f\t", val);

        	ioctl(fd, 17, &iRes);                   /* 电池电压默认除4    */
		val = (iRes * 7.4) / 4096.0;
        	printf("Vbat:%.2f\t", val);

		printf("\n");
	}
	close(fd);
}
