//main.h各文件公用的头文件
//
#ifndef _MAIN_H
#define _MAIN_H

#define NUM 10

#include "stdio.h"
#include "stdlib.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "memory.h"
#include "stdbool.h"
#include "assert.h"
#include <sys/stat.h>
#include <pthread.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "string.h"
#include <semaphore.h>
//#include <rw_semaphore.h>

typedef struct
{
	unsigned char *start;//地址指向映射后的内核帧缓冲区
	size_t offset;//内核帧缓冲区相对文件句柄的偏移
	size_t length;//记录内存映射了多大空间，最后删除需要用到
    int bufnum;

    unsigned char* picStart;
    int imageSize;
}videoBufType;

typedef struct
{
    sem_t sem_tower_wakeup;//本地jpg读写的信号量
    unsigned long hori_angle;
    unsigned long veri_angle;
    bool b_tower_running;
}tower_info_type;

typedef struct
{
    sem_t sem_input_wakeup;//本地jpg读写的信号量
    sem_t sem_wr_enable;//本地jpg读写的信号量

    bool b_input_running;
}input_info_type;

typedef struct
{
    char name[48];//要写入的文件名,为了字节对齐取48
    int count;//要写入多少张
    
    //此用法是posix的无名信号量头文件是：<semaphore.h>
    sem_t sem_wtofile_wakeup;//本地jpg读写的信号量
    sem_t sem_wtofile_standby;//本地jpg读写的信号量

    bool b_wtofile_running;
    bool b_finish_wtofile;
}wtofile_info_type;

typedef struct
{
    //此用法是posix的无名信号量头文件是：<semaphore.h>
    sem_t sem_wtolcd_wakeup;//写到LCD的进程唤醒信号量
    sem_t sem_wtolcd_standby;//image图像准备好，唤醒信号量

    bool b_wtolcd_running;
    bool b_finish_wtolcd;
}wtolcd_info_type;

typedef struct
{
    //此用法是posix的无名信号量头文件是：<semaphore.h>
    sem_t sem_deal_wakeup;//本地jpg读写的信号量
    sem_t sem_deal_standby;//本地jpg读写的信号量

    bool b_deal_running;
    bool b_finish_deal;
}deal_info_type;

typedef struct
{
    sem_t shmSem;

    tower_info_type tower;//舵机控制进程的共享内存区
    input_info_type input;//图像输入进程的共享内存区
    wtofile_info_type wtofile;//图像保存的共享内存区
    wtolcd_info_type  wtolcd;//实时图像显示的控制共享内存区
    deal_info_type    deal;

    bool b_endflag;//所有进程的控制结束标志
}shmType;

extern bool chartoint(char* str, int* result);

#endif
