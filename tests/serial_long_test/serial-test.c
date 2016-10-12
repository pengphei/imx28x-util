/*********************Copyright(c)************************************************************************
**          Guangzhou ZHIYUAN electronics Co.,LTD
**
**              http://www.embedtools.com
**
**-------File Info---------------------------------------------------------------------------------------
** File Name:               serial-test.c
** Latest modified Data:    2008-05-19
** Latest Version:          v1.1
** Description:             NONE
**
**--------------------------------------------------------------------------------------------------------
** Create By:               zhuguojun
** Create date:             2008-05-19
** Version:                 v1.1
** Descriptions:            epc-8000's long time test for serial 1,2,3,4
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>   
#include <limits.h> 
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>

#define DATA_LEN                0x08                                    /* test data's len              */

#define STOP                    0                                       /* test stop                    */
#define RUN                     1                                       /* test running                 */

static int GiRunStatue;                                                 /* test pthread status          */

#define DEBUG                 1

static int GiSerialFds[4] = {-1, -1, -1, -1};                           /* all serial device fd         */

/*********************************************************************************************************
** Function name:           reportString
** Descriptions:            write string to file: report.txt. if the file not exit, the function will
**                          create it.
** input paramters:         cStr        String will be writed to file
** output paramters:        NONE
** Return value:            NONE
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void reportString(char *cStr)
{
    int iFd;

    iFd = open("report.txt", O_APPEND | O_RDWR | O_CREAT, 
                S_IRUSR | S_IWUSR |  S_IRGRP | S_IROTH);
   
    write(iFd, cStr, strlen(cStr));

    close(iFd);
}

/*********************************************************************************************************
** Function name:           openSerial
** Descriptions:            open serial port at raw mod
** input paramters:         iNum        serial port which can be value at: 1, 2, 3, 4
** output paramters:        NONE
** Return value:            file descriptor
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static int openSerial(int iPort)
{
    int iFd;

    struct termios opt; 
    char cSerialName[15];

    if (iPort >= 10) {
        printf("no this serial:ttySP%d . \n", iPort);
        exit(1);
    }
    
    if (GiSerialFds[iPort - 1] > 0) {
        return GiSerialFds[iPort - 1];
    }

    sprintf(cSerialName, "/dev/ttySP%d", iPort - 1);
    printf("open serila name:%s \n", cSerialName);
    iFd = open(cSerialName, O_RDWR | O_NOCTTY);                        
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt);      

    //cfsetispeed(&opt, B9600);
    //cfsetospeed(&opt, B9600);

     cfsetispeed(&opt, B115200);
     cfsetospeed(&opt, B115200);

    
    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   DATA_LEN;                                      
    opt.c_cc[VTIME]  =   0;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }

    GiSerialFds[iPort - 1] = iFd; 

    return iFd;
}

/*********************************************************************************************************
** Function name:           setData
** Descriptions:            init test-data with rand data
** input paramters:         ucValues        buff which data need inited
**                          iLen            data's len in buff
** output paramters:        ucValues        buff which data have inited
** Return value:            NONE
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void setData(unsigned char *ucValues, int iLen)
{
    int i;
    static int iGerm;

    if (ucValues == NULL) {
        printf("error: uninit pointer: ucValues in serial-test.c::setData. \n");
    }

    srand(iGerm);
    iGerm++;
    
    for (i = 0; i < iLen; i++) {
        //ucValues[i] = (int)(256.0*rand()/(RAND_MAX + 1.0)); 
        ucValues[i] = i%0xff; 
    }
}

/*********************************************************************************************************
** Function name:           testSerial
** Descriptions:            test serial for 20 times
**                          This function test two serail port. One serail port send data, and other 
**                          serial port recevie data. It will operation for 20 times.
** input paramters:         iFdsend         serial port which send data
**                          iFdRec          serial port which receive date
**                          iTimes          times of test has done
** output paramters:        NONE
** Return value:            0               test success
**                          others          test faile
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static struct timeval tv_timeout;
static fd_set fs_read;
static int testSerial(int iPortSend, int iPortRec, int iTimes)
{
    unsigned char ucDataSend[DATA_LEN], ucDataGet[DATA_LEN];
    int iRet = 1; 
    int i, j;

    char cStr[150];
    time_t  timeP;
    int iFdSend, iFdRec;

    /*
     * get descriptor of serial port while will send data
     */
    iFdSend = openSerial(iPortSend);
    /*
     * get descriptor of serial port while will receive data
     */
    iFdRec  = openSerial(iPortRec);


    /*
     * test serial port send/receive for 20 times
     */
    for (i = 0; i < 2; i++) {

        setData(ucDataSend, DATA_LEN);                                  /* init send data randily       */

        write(iFdSend, ucDataSend, DATA_LEN);                           /* send data to other serial    */
        /*
         *  get data at the other serail port
         *  if time out for 2 second, report error
         */ 
        FD_ZERO(&fs_read);
        FD_SET (iFdRec, &fs_read);

        tv_timeout.tv_sec = 0;
        tv_timeout.tv_usec = 2000000;
        iRet = select(iFdRec + 1, &fs_read, NULL, NULL, &tv_timeout); 

        if (iRet) {
            iRet = read(iFdRec, ucDataGet, DATA_LEN);                          /* receive data                 */


            /*
             *  compare send data and receive data. if send data and receive are unequal,
             *  report error.
             */ 
            for (j = 0; j < DATA_LEN; j++) {
		int i = 0;
                if (ucDataSend[j] != ucDataGet[j]) {
                    printf(" data error ,send port: %d receive port:%d \n", iPortSend, iPortRec);

                    sprintf(cStr, "\ndate test error, send port: %d  receive port: %d \n",
                            iPortSend, iPortRec);
                    reportString(cStr);

                    sprintf(cStr, "at times: %d \n", iTimes);
                    reportString(cStr);

                    sprintf(cStr, "error time: %s \n ", asctime(gmtime(&timeP)) );
                    reportString(cStr);
#ifdef DEBUG
	    printf("get data:%d \n", iRet); 
            for (i = 0; i < iRet; i++) {
		if (i%15 == 0) 
			printf("\n");
                printf(" %x %x,", ucDataSend[i], ucDataGet[i]);
            }
#endif
                    return -1;
                }
            }

        } else {                                                    
            /*
             *  if time out, report error
             */

            printf("time out send port: %d receive port: %d \n", iPortSend, iPortRec);

            sprintf(cStr, "\ntime out error, send port: %d  receive port: %d \n",
                            iPortSend, iPortRec);
            reportString(cStr);

            sprintf(cStr, "at times: %d \n", iTimes);
            reportString(cStr);

            sprintf(cStr, "error time: %s \n ", asctime(gmtime(&timeP)) );
            reportString(cStr);

            return -2;
        }
    }

    return 0;
}

/*********************************************************************************************************
** Function name:           stopTest
** Descriptions:            wait for user input "stop" to stop test.
**                          when user input "stop", the function will set GiRunStatue as STOP, and the 
**                          serila long times test pthread will finish.
** input paramters:         iParam          private data
** output paramters:        NONE
** Return value:            0               success
**                          other           faile
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static int stopTest(int *iParam)
{
    char cStr[20];

    while(strcmp(cStr, "stop") != 0) {
        scanf("%s", cStr);
    }       

    printf("serial test finish.... \n");

    GiRunStatue = STOP;

    return 0;
}

/*********************************************************************************************************
** Function name:           main
** Descriptions:            main function
** input paramters:         NONE
** output paramters:        NONE
** Return value:            0       success
**                          other   faile
** Create by:               zhuguojun
** Create Data:             2008-05-19
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
char Usage[] = {
"**********************************************\r\n \
Usage:\r\n \
txd0 connected to rxd1 \r\n \
rxd0 connected to txd1\r\n \
then run \r\n \
./serial-test\r\n \
to start the demo \r\n \
input 'stop' or 'ctl+c' to quit the demo\r\n \
***********************************************\r\n \
"
};
int main(void)
{
    int iRet1, iRet2, iRet3, iRet4;
    int i, iSuccess, iFalie;
    pthread_t threadNew;
    int iT = 0;

    char    cStr[150];
    time_t  timeP;
    time_t  timeUse;
    time_t  timeStart;
    struct  tm *p;
    printf(Usage);
    /*
     *  make a report's title and report the test start time
     */
    reportString("\n\n********************************************* \n\n");
    time(&timeP);
    time(&timeStart);
    sprintf(cStr, "start time: %s \n", asctime(gmtime(&timeP)) );
    reportString(cStr);
    
    
    /*
     *  open serail device file needed tested.
     */
    openSerial(1);
    openSerial(2);
//    openSerial(3);
//    openSerial(4);

    /*
     *  clean the test status flag
     */
    iSuccess = 0;
    iFalie   = 0;

    /*
     *   create a pthread which waite for user input "stop" and stop the test
     */
    GiRunStatue = RUN;
    iRet1 = pthread_create(&threadNew, PTHREAD_CREATE_JOINABLE, (void *)stopTest, NULL);
    if (iRet1) {
        perror("pthread_creae: threadNew. \n");
        return 0;
    }

    i = 0;
    while(1) {
        i++;

        iRet1 = testSerial(1, 2, i);                                    /* test serial 1, 2. serail 1   */
                                                                        /* send and serail 2 receive    */
        iRet2 = testSerial(2, 1, i);                                    /* test serial 2, 1.            */


        /*
         *  if all serial test pass, test success flag count    
         *  else test faile flag count
         */
        if ( (iRet1 + iRet2) == 0) {                   
           iSuccess++; 
        } else {
            iFalie++;
	    if (iFalie > 10)  {
			printf("O no ! faile!~~! \n");
			return -1;	
		}
        }

        /*
         *  tell user how the test going
         */
        printf("serial test %d times,failed %d times \n",i, iFalie);

        /*
         *  tell user how much time pass
         */
        time(&timeP);
        timeUse = timeP - timeStart;
        p = gmtime(&timeUse);
        printf("use time: %d-%d %d:%d:%d \n", p->tm_mon, p->tm_mday - 1, 
                                    p->tm_hour, p->tm_min, p->tm_sec);
        /*
         *  report the test result every hour
         */
        if (iT != p->tm_hour) {
            iT = p->tm_hour;

            sprintf(cStr, "\nsuccess times: %d", iSuccess);
            reportString(cStr);

            sprintf(cStr, "\nfaile times:   %d", iFalie);
            reportString(cStr);

            time(&timeP);
            sprintf(cStr, "\nend time: %s ", asctime(gmtime(&timeP)) );
            reportString(cStr);

            time(&timeP);
            timeUse = timeP - timeStart;
            p = gmtime(&timeUse);
            sprintf(cStr, "\nuse time: %d-%d %d:%d:%d \n", p->tm_mon, p->tm_mday - 1, 
                                    p->tm_hour, p->tm_min, p->tm_sec);
            reportString(cStr);
        }

        /*
         *  finish the test if user have input "stop"
         */
        if (GiRunStatue == STOP) {
            break;
        }
    }

    /*
     * report the test result
     */
    sprintf(cStr, "\nsuccess times: %d", iSuccess);
    reportString(cStr);

    sprintf(cStr, "\nfaile times:   %d", iFalie);
    reportString(cStr);

    time(&timeP);
    sprintf(cStr, "\nend time: %s ", asctime(gmtime(&timeP)) );
    reportString(cStr);

    time(&timeP);
    timeUse = timeP - timeStart;
    p = gmtime(&timeUse);
    sprintf(cStr, "\nuse time: %d-%d %d:%d:%d \n", p->tm_mon, p->tm_mday - 1, 
                                    p->tm_hour, p->tm_min, p->tm_sec);
    reportString(cStr);


    reportString("\n---------------------------------------------\n");

    printf("please check the test report at report.txt \n");
    
    return 0;
}


/*********************************************************************************************************
    end file
*********************************************************************************************************/
