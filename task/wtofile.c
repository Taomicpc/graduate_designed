//将内核映射内存区的图像数据存储成文件格式
#include "public.h"

int main(int argc, char* argv[])
{
    int IshmId;
    key_t ipcKey;
    char filename[50];
    FILE* writeImageFd = 0;
    shmType* shmPtr = NULL;
    int jpgnum;
    int count;

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

     
    count = 0;
    while(1)
    {
        //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            break;
        }
        
        if(shmPtr->wtofile.b_wtofile_running == false)
        {
            sem_wait(&shmPtr->wtofile.sem_wtofile_wakeup);//睡眠等待控制台允许
        }

        jpgnum = shmPtr->wtofile.count;
        
        for(count=0; count<jpgnum;)
        {    
            sem_wait(&shmPtr->wtofile.sem_wtofile_standby);//等待获得写入文件的信号量
    
            if(shmPtr->b_endflag == true)
            {//主程序改变标志位，需要退出
                break;
            }
            
            sprintf(filename, "%s%d.jpg", shmPtr->wtofile.name, count);
            //printf("writing %s from 0x%x,jpg size:%d\n", filename, shmPtr->videoBuf[0].picStart,shmPtr->videoBuf[0].imageSize);
            char command[50];
            sprintf(command, "cp -f ./image/image.jpg ./image/%s", filename);
            printf("prepera to save %s\n",filename);
    
            if(fork() == 0)
                system(command);
            else
            {//父进程
                int status;
                wait(&status);//等待子进程结束
                printf("children process end.return %d\n", status);
                ++count;
           
                sem_wait(&shmPtr->shmSem);//获得共享内存的使用权利
                shmPtr->wtofile.b_finish_wtofile= true;
                shmPtr->wtofile.b_wtofile_running = false;
                sem_post(&shmPtr->shmSem);//释放共享内存区信号量
                //pthread_rwlock_rdlock(&dataPtr->rwlock);//上读锁
                sleep(1);//延时１s           
            
                sem_wait(&shmPtr->shmSem);//获得共享内存的使用权利
                shmPtr->wtofile.b_wtofile_running = true;
                //printf("wakeup! %d\n", shmPtr->b_finishwtofile);
                sem_post(&shmPtr->shmSem);//释放共享内存区信号        
                continue;//跳过后面代码继续while循环
            }
    
            exit(12);//shell命令子进程返回
        }

        //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            break;
        }
   
        printf("Have save %d picture\n",jpgnum);
        
        sem_wait(&shmPtr->shmSem);//获得共享内存的使用权利
    
        shmPtr->wtofile.b_wtofile_running = false;//写文件任务完成，退出
        shmPtr->wtofile.b_finish_wtofile = true;
    
        sem_post(&shmPtr->shmSem);//释放共享内存区信号        
    }

    shmdt(shmPtr);//解除映射关系;

    return 0;
}

