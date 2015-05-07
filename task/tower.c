//舵机的应用层测试函数
//2015-3-30
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "assert.h"
#include "myLib.h"
#include "public.h"
#include <unistd.h>

#define S_HORIZON 1
#define S_VERTICAL 0

//ioctl的指令
#define HORIZON (0x1<<31)
#define VERTIAL (0x0)

#define HORIZON_ON (HORIZON | 0x1)
#define HORIZON_OFF (HORIZON & (~0x1))
#define HORIZON_SET (HORIZON_ON | (0x1<<1))

#define VERTIAL_ON (VERTIAL | 0x1)
#define VERTIAL_OFF (VERTIAL & (~0x1))
#define VERTIAL_SET (VERTIAL_ON | (0x1<<1))
//读寄存器的值
#define HORI_MAX_VALUE 2528
#define HORI_MIN_VALUE 487
#define HORI_VALUE_PER_DEGREE (HORI_MAX_VALUE - HORI_MIN_VALUE)/180//１度代表的pwm脉宽，特殊用法，带入后先算乘

#define VERT_MAX_VALUE 2558
#define VERT_MIN_VALUE 596 
#define VERT_VALUE_PER_DEGREE (VERT_MAX_VALUE - VERT_MIN_VALUE)/180//１度代表的pwm脉宽

//把字符数转换成整形数
int ctoi(char str[])
{
    int result=0;
    int i=0;

    assert(str != NULL);

    while(str[i]>='0' && str[i]<='9')
    {
        result = result*10 + str[i] - '0' ;
        ++i;
    }

    return result;
}

int main(int argc, char** argv)
{   
    int IshmId;
    key_t ipcKey;
    shmType* shmPtr = NULL;
    int pre_hori_angle = 0;
    int pre_vert_angle = 0;
    int hori_pwm_value = 1000;
    int vert_pwm_value = 1000;

    struct Angle
    {
        unsigned long horiAngle ;
        unsigned long verAngle ;
    }angle;
    
    int fd=3;

//连接到共享内存    
    ipcKey = ftok("/opt/designed/shm", 'a');
    if(ipcKey == -1)
        perror("ftok error");

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666是ubuntu下此操作需要的校验

    if(IshmId == -1)
    {
        perror("shmget error");
        return;
    }

    shmPtr = (shmType *)shmat(IshmId, NULL, 0);
   
    if(shmPtr == (void *)-1)
    {
        perror("main.c shmat error");
        return;
    }

//舵机部分
    fd=open("/dev/taoTower",O_RDWR);
    if(fd<0)
    {
        printf("taoTower driver open error");
        exit(1);
    }

    //初始化角度为９０度左右
    ioctl(fd, HORIZON_SET, (HORI_MAX_VALUE+HORI_MIN_VALUE)/2 );     
    ioctl(fd, VERTIAL_SET, (VERT_MAX_VALUE+VERT_MIN_VALUE)/2 );     

    printf("Tower process standby!\n");

    while(1)
    {
        //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            //sem_post(&shmPtr->shmSem);
            break;
        }
        
        if(shmPtr->tower.b_tower_running == false)
        {
            printf("Tower process Sleep!\n");
            sem_wait(&shmPtr->tower.sem_tower_wakeup);//睡眠等待控制台允许
            printf("Tower process Wakeup!\n");
        }
    
//应用层直接赋值，由底层实现加减速
        hori_pwm_value = HORI_MAX_VALUE - (shmPtr->tower.hori_angle * HORI_VALUE_PER_DEGREE);
        vert_pwm_value = VERT_MAX_VALUE - (shmPtr->tower.veri_angle * VERT_VALUE_PER_DEGREE);

        ioctl(fd, HORIZON_SET, hori_pwm_value);     
        ioctl(fd, VERTIAL_SET, vert_pwm_value);     

        sem_wait(&shmPtr->shmSem);
        shmPtr->tower.b_tower_running = false; 
        sem_post(&shmPtr->shmSem);
    }   
    close(fd);
    shmdt(shmPtr);//解除映射关系;
    printf("tower process exit!\n");
    
    return 0;
}

