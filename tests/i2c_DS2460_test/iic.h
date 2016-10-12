#ifndef __SERIAL_H
#define __SERIAL_H

#define I2C_SLAVE	0x0703
#define I2C_TENBIT	0x0704

int GiFd;

int GiIndex;

int  iicOpen();

int waitIRQ();
void waitset();

void setdriverTo500smod();
void setdriverToCOMMmod();

#endif



