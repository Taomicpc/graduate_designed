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
#define S_HORIZON 1
#define S_VERTICAL 0

#define HORIZON (0x1<<31)
#define VERTIAL (0x0)

#define HORIZON_ON (HORIZON | 0x1)
#define HORIZON_OFF (HORIZON & (~0x1))
#define HORIZON_SET (HORIZON_ON | (0x1<<1))

#define VERTIAL_ON (VERTIAL | 0x1)
#define VERTIAL_OFF (VERTIAL & (~0x1))
#define VERTIAL_SET (VERTIAL_ON | (0x1<<1))
//读寄存器的值
//
#define S3C6410_TCMPB0 0 
#define S3C6410_TCMPB1 1
#define CHANGE_STEP 50
//状态机的状态
#define INIT_STATE 0
#define MENU_STATE 1
#define SET_ANGLE_STATE 2
#define CTL_ANGLE_STATE 3
#define EXIT_STATE 4

int main(int argc, char** argv)
{   
    int IshmId;
    key_t ipcKey;
    shmType* shmPtr = NULL;
    
    struct Angle
    {
        unsigned long horiAngle ;
        unsigned long verAngle ;
    }angle;
    
    int fd=3;

//连接到共享内存    
    ipcKey = ftok("./shm", 'a');
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
        printf("API join error");
        exit(1);
    }

    ioctl(fd, HORIZON_SET, shmPtr->tower.hori_angle);     
    ioctl(fd, VERTIAL_SET, shmPtr->tower.veri_angle);     

    printf("\nHello!Welcome to tao's tower control!\n");
    printf("you could input how you want to control\n");

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
            sem_wait(&shmPtr->tower.sem_tower_wakeup);//睡眠等待控制台允许
        }
        
        ioctl(fd, HORIZON_SET, shmPtr->tower.hori_angle);     
        ioctl(fd, VERTIAL_SET, shmPtr->tower.veri_angle);     
    
        sem_wait(&shmPtr->shmSem);
        shmPtr->tower.b_tower_running = false; 
        sem_post(&shmPtr->shmSem);
    }   
    close(fd);
    shmdt(shmPtr);//解除映射关系;
    
    return 0;
}

