/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                               http://www.zlg.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               wdt_test.c
** Last modified Date:      2015.1.6
** Last Version:            1.00
** Description:             On chip watchdog test routine
**
**--------------------------------------------------------------------------------------------------------
** Created By:              hak
** Created date:            2015.1.6
** Version:                 1.00
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

/*********************************************************************************************************
全局变量定义
*********************************************************************************************************/
static const char *device = "/dev/watchdog";                             /* 设备驱动文件路径             */
static int enable    = 1;                                                /* 执行程序默认使能看门狗       */
static int timeout   = 20;                                               /* 默认配置的看门狗溢出时间     */
static int keepalive = 5;                                                /* 默认进行5秒喂狗演示          */

/*********************************************************************************************************
函数声明
*********************************************************************************************************/
void print_usage(const char *prog);
void parse_opts(int argc, char *argv[]);

/*********************************************************************************************************
** Function name:           main
** Descriptions:            主函数
** input parameters:        argc:命令行参数个数; argv:参数列表
** output parameters:       无
** Returned value:          0:正常退出 other:异常退出
** Created By:              hak
** Created date:            2015.1.6
*********************************************************************************************************/
int main(int argc, char *argv[])
{
    int fd;
    int ret = 0;
    int i = 0;
    int opts = 0;
    
    parse_opts(argc, argv);                                              /* 读取并解释命令行参数         */
    fd = open(device, O_RDWR);                                           /* 读写方式打开设备文件         */
    if (fd < 0) {
        printf("device open failed!\r\n");                               /* 设备文件打开失败             */
        return -1;
    }
    //设备文件打开后看门狗随机被使能，溢出时间为内核中记录的上一次的设置值，首次设置默认为19秒
    printf("device open succeed, watchdog enable, "\
           "system will be restart in %d second if not keep alive!\r\n", timeout);
    if (enable == 0) {                                                   /* 禁能看门狗                   */
        // 禁能看门狗前需先写入'V'(WDT_OK_TO_CLOSE)，防止误关闭
        if (write(fd, "V", 1) != 1) {
            printf("write WDT_OK_TO_CLOSE failed!");
        }
        opts = WDIOS_DISABLECARD;
        ret = ioctl(fd, WDIOC_SETOPTIONS, &opts);                        /* 关闭看门狗                   */
        printf("watchdog disable!\r\n");
    } else {                                                             /* 设置看门狗加载值             */
        ioctl(fd, WDIOC_SETTIMEOUT, &timeout);                           /* 设置新的溢出时间             */
        if (keepalive > 0) {
            for (i = 0; i < keepalive; i++) {
                ret = ioctl(fd, WDIOC_KEEPALIVE, 0);                     /* 喂狗操作                     */
                sleep(1);                                                /* 延时1秒                      */
                printf("keep alive:%d s, ioctl ret = %d\r\n", i + 1, ret);
            }
        }
    }
    
    ret = close(fd);                                                     /* 关闭设备文件                 */
    if (ret != 0) {
        printf("device closed unexpectdly, watchdog keep on!\r\n");
    }
    return ret;
}

/*********************************************************************************************************
** Function name:           print_usage
** Descriptions:            程序命令行参数用法提示
** input parameters:        prog:程序描述字符串
** output parameters:       无
** Returned value:          无
** Created By:              hak
** Created date:            2015.1.6
*********************************************************************************************************/
void print_usage(const char *prog)
{
    printf("Usage: %s [-desk]\r\n", prog);
    puts("  -d --device       device to use (default /dev/watchdog)\r\n"
         "  -e --enable       0 = disable, 1 = enable(default)\r\n"
         "  -s --timeout      seconds for timeout setting, 1~4194304(default 20)\r\n"
         "  -k --keepalive    seconds for keep alive test show, 0~4194304(default 5)\r\n");
    exit(1);
}

/*********************************************************************************************************
** Function name:           parse_opts
** Descriptions:            解释并处理命令行参数
** input parameters:        argc:命令行参数个数; argv:参数列表
** output parameters:       无
** Returned value:          无
** Created By:              hak
** Created date:            2015.1.6
*********************************************************************************************************/
void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            {"device",    1, 0, 'd'},
            {"enable",    1, 0, 'e'},
            {"timeout",   1, 0, 's'},
            {"keepalive", 1, 0, 'k'},
            {NULL, 0, 0, 0},
        };
        int c;
        
        c = getopt_long(argc, argv, "d:e:s:k:", lopts, NULL);
        
        if (c == -1)
        break;
        
        switch (c) {
            case 'd':
            device = optarg;
            break;
            case 'e':
            enable = atoi(optarg);
            if ((enable < 0) || (enable > 1)) {
                printf("enable parameter error,it must be 0 or 1\r\n");
                exit(1);
            }
            break;
            case 's':
            timeout = atoi(optarg);
            if ((timeout <= 0) || (timeout > (0x10000000 >> 6))) {
                printf("timeout parameter error,it must be 1 to 4194304\r\n");
                exit(1);
            }
            break;
            case 'k':
            keepalive = atoi(optarg);
            if ((keepalive < 0) || (keepalive> (0x10000000 >> 6))) {
                printf("keepalive parameter error,it must be 0 to 4194304\r\n");
                exit(1);
            }
            break;
            default:
            print_usage(argv[0]);
            break;
        }
    }
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
