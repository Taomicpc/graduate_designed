//图像采集处理的主程序，由此进程初始化
//

#include "public.h"
#include "myLib.h"

//状态机的状态
#define INIT_STATE 0
#define MENU_STATE 1

//舵机私有状态
#define SET_ANGLE_STATE 2
#define CTL_ANGLE_STATE 3
#define EXIT_STATE 4
#define CHANGE_STEP 50

void set_shm_angle(shmType* shmPtr, unsigned long horiAngle, unsigned long verAngle);
void towerctl(shmType* shmPtr);
//传入参数：第一个是要写的jpeg文件名，第二个是要记录的文件个数
int main(int argc, char* argv[])
{
//--------------共享内存创建--------------------
    key_t ipcKey;
    int IshmId;
    shmType* shmPtr=NULL;

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

    memset(shmPtr, 0, sizeof(shmType));
    //pthread_rwlock_init(&shmPtr->rwlock, NULL);//读写锁初始化
    sem_init(&shmPtr->shmSem, 1, 1);//前一个1表明在进程间使用，后一个1设置一个信号量

//回收各进程启动运行的信号量，如果不给信号，进程将进入睡眠状态．
    sem_init(&shmPtr->input.sem_input_wakeup, 1, 0);//前一个1表明在进程间使用，后一个1设置一个信号量
    sem_init(&shmPtr->wtofile.sem_wtofile_wakeup, 1, 0);//设忙碌，由写信号去释放其信号量
    sem_init(&shmPtr->wtolcd.sem_wtolcd_wakeup, 1, 0);//设忙碌，由写信号去释放其信号量
    sem_init(&shmPtr->deal.sem_deal_wakeup, 1, 0);//设忙碌，由写信号去释放其信号量
    sem_init(&shmPtr->tower.sem_tower_wakeup, 1, 0);//设忙碌，由写信号去释放其信号量

    //与之相对应的布尔量初始化
    shmPtr->input.b_input_running = false;//先置标志为，使各进程进入上面wakeup信号量的睡眠状态 
    shmPtr->wtofile.b_wtofile_running = false;//先置标志为，使各进程进入上面wakeup信号量的睡眠状态 
    shmPtr->wtolcd.b_wtolcd_running = false;//先置标志为，使各进程进入上面wakeup信号量的睡眠状态 
    shmPtr->deal.b_deal_running = false;//先置标志为，使各进程进入上面wakeup信号量的睡眠状态 
    shmPtr->tower.b_tower_running = false;//先置标志为，使各进程进入上面wakeup信号量的睡眠状态 

//回收更新image.jpg后赋予的信号量
    sem_init(&shmPtr->input.sem_wr_enable, 1, 0);//前一个1表明在进程间使用，后一个1设置一个信号量
    sem_init(&shmPtr->wtofile.sem_wtofile_standby, 1, 0);//设忙碌，由写信号去释放其信号量
    sem_init(&shmPtr->wtolcd.sem_wtolcd_standby, 1, 0);//设忙碌，由写信号去释放其信号量
    sem_init(&shmPtr->deal.sem_deal_standby, 1, 0);//设忙碌，由写信号去释放其信号量
    //与之相对应布尔量初始化
    shmPtr->wtofile.b_finish_wtofile = true;//各进程退出命令，true为退出
    shmPtr->wtolcd.b_finish_wtolcd = true;
    shmPtr->deal.b_finish_deal = true;

//-------------创建进程-------------------
    char SshmId[20];//用于接收字串型的shmId
//    char SsemId[20];//用于接收字串型的shmId

    sprintf(SshmId, "%d", IshmId);
//    sprintf(SsemId, "%d", IsemId);

    char* execvInput[] = {"./input", SshmId, NULL};
    char* execvWtoFile[] = {"./wtofile", SshmId, argv[1], argv[2], NULL};
    char* execvWtoLCD[] = {"./wtolcd", SshmId, NULL};
    char* execvDeal[] = {"./deal", SshmId, NULL};
    char* execvTower[] = {"./tower", SshmId, NULL};

    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execv("./tower", execvTower, NULL);//启动共享资源写入进程
        perror("execv tower error");
    }
    
    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execv("./input", execvInput, NULL);//启动共享资源写入进程
        perror("execv input error");
    }

    if(fork()==0)//缓冲区数据显示到lcd
    {
        execv("./wtofile", execvWtoFile, NULL);//启动共享资源写入进程
        perror("execv wtofile error");
    }

    if(fork()==0)//缓冲区数据显示到lcd
    {
        execv("./wtolcd", execvWtoLCD, NULL);//启动共享资源写入进程
        perror("execv wtolcd error");
    }

    if(fork()==0)//图像处理进程
    {
        execv("./deal", execvDeal, NULL);//启动共享资源写入进程
        perror("execv deal error");
    }

    int state;
    int endflag = false;

    while(endflag == false)
    {
        switch(state)
        {
            case INIT_STATE:
            {
                printf("1.Open reflash to lcd\n");
                printf("2.Close reflash to lcd\n");
                printf("3.Save picture to arm\n");
                printf("4.Show deal picture\n");
                printf("5.Ctrl tower angle\n");
                printf("q.To exit program or back to pre_menu\n\n");
                printf("Your choice:");
                
                 sem_wait(&shmPtr->shmSem);
                 shmPtr->input.b_input_running = true;
                 sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
                 sem_post(&shmPtr->shmSem);

                state = MENU_STATE;
            }break;

            case MENU_STATE:
            {//主目录输入状态
                char savename[50];
                int number=0;
                char choice = myGetchar();//非阻塞获取
                putchar(choice);//立即回显
        
                switch(choice)
                {
                    case '1'://刷新采集图像到lcd
                        sem_wait(&shmPtr->shmSem);
                        shmPtr->wtolcd.b_wtolcd_running = true;
                        shmPtr->deal.b_deal_running = false;
                        sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//开始video更新
                        sem_post(&shmPtr->shmSem);
                        state = INIT_STATE;
                        break;

                    case '2'://关闭lcd刷新显示
                        sem_wait(&shmPtr->shmSem);
                        shmPtr->wtolcd.b_wtolcd_running = false;
                        sem_post(&shmPtr->shmSem);
                        state = INIT_STATE;
                        break;

                    case '3'://保存图像 
                        printf("Input save name and number:");
                        scanf("%s %d", savename, &number);
                        printf("savename = %s, number = %d \n", savename, number);

                        sem_wait(&shmPtr->shmSem);
                        strcpy(shmPtr->wtofile.name, savename);
                        shmPtr->wtofile.count = number;
                        shmPtr->wtofile.b_wtofile_running = true;
                        sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
                        sem_post(&shmPtr->shmSem);
                        state = INIT_STATE;
                        break;

                    case '4'://改为刷新显示处理后的图像到lcd
                        sem_wait(&shmPtr->shmSem);
                        shmPtr->wtolcd.b_wtolcd_running = false;
                        shmPtr->deal.b_deal_running = true;
                        sem_post(&shmPtr->deal.sem_deal_wakeup);//开始video更新
                        sem_post(&shmPtr->shmSem);
                        state = INIT_STATE;
                        break;
                    case '5'://舵机角度控制菜单进入
                        towerctl(shmPtr);
                        state = INIT_STATE;
                        break;
                    case 'q':
                        printf("\nThank you for use\nBye!\n");
                        endflag = true; 
                        break;
                    default:
                         printf("1.Open reflash to lcd\n");
                         printf("2.Close reflash to lcd\n");
                         printf("3.Save picture to arm\n");
                         printf("4.Show deal picture\n");
                         printf("5.Ctrl tower angle\n");
                         printf("q.To exit program or back to pre_menu\n\n");
                         printf("Your choice:");
                         break;
                }
            }break;

        default:
            state = INIT_STATE;
            break;
        }
    }

    sem_wait(&shmPtr->shmSem);//修改共享内存内容前获得信号量
    shmPtr->b_endflag = true;//置共享内存处的退出标志为为真，各进程读取后退出当前进程。

    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
    sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_wakeup);//开始video更新
    sem_post(&shmPtr->tower.sem_tower_wakeup);//开始video更新
    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->wtofile.sem_wtofile_standby);//开始video更新
    sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_standby);//开始video更新

    sem_post(&shmPtr->shmSem);//释放信号量

    int status;

    while(wait(&status) != -1);//一直等待其产生的子进程都结束,每一次等待都是休眠

    printf("Main exit\n");
    sem_destroy(&shmPtr->shmSem);//销毁无名信号量
    shmctl(IshmId, IPC_RMID, 0);//删除共享映射区
//    pthread_rwlock_destroy(&shmPtr->rwlock);//销毁信号量
}

void set_shm_angle(shmType* shmPtr, unsigned long horiAngle, unsigned long verAngle)
{
     sem_wait(&shmPtr->shmSem);

     shmPtr->tower.hori_angle = horiAngle;
     shmPtr->tower.veri_angle = verAngle;
     shmPtr->tower.b_tower_running = true;
     sem_post(&shmPtr->tower.sem_tower_wakeup);
     
     sem_post(&shmPtr->shmSem);
}

void towerctl(shmType* shmPtr)
{
    int state = INIT_STATE;
    char choice;
    
    struct Angle
    {
        unsigned long horiAngle ;
        unsigned long verAngle ;
    }angle;

    while(1)
    {
        switch(state)
        {
            case INIT_STATE:
            {
                printf("1.Input num to set tower angle\n");
                printf("2.Use a,s,d,w to control constantly\n");
                printf("q.To exit program or back to pre_menu\n\n");
                printf("Your choice:");
                state = MENU_STATE;
            }break;

            case MENU_STATE:
            {//主目录输入状态
                char choice = myGetchar();//非阻塞获取
                putchar(choice);//立即回显
        
                switch(choice)
                {
                    case '1':
                        state = SET_ANGLE_STATE;
                        break;
                    case '2':
                        state = CTL_ANGLE_STATE;
                        printf("\nPress \na--right\nd--lef\ns--down\nw--up\n");
                        break;
                    case 'q':
                        printf("\nThank you for use\nBye!\n");
                        return; 
                        break;
                    default:
                        printf("\nInput error try again\n");
                        printf("1.Input num to set tower angle\n");
                        printf("2.Use a,s,d,w to control constantly\n");
                        printf("q.To exit program or back to pre_menu\n\n");
                        printf("Your choice:");
                        break;
                }
            }break;

            case SET_ANGLE_STATE:
            {//直接输入角度控制状态
                printf("\nPlease input two num(500~2000) like (1000 1500):");

                while(scanf("%d %d", &angle.horiAngle, &angle.verAngle)!=2)
                {
                    if((choice = getchar()) == 'q')
                    {
                        printf("Back to pre_menu\n\n");
                        state = INIT_STATE;
                        break;
                    }
                   // printf("\nInput error! input again:");
                }
                if(choice != 'q')
                {
                    set_shm_angle(shmPtr, angle.horiAngle, angle.verAngle);
                }
            }break;

            case CTL_ANGLE_STATE:
            {//a,s,d,w上下左右控制状态
                choice = myGetchar();
                switch(choice)
                {
                case 'w':
                    printf("UP!!!\n");
                    angle.verAngle = shmPtr->tower.veri_angle;//读取TCMP1
                    if((angle.verAngle+CHANGE_STEP) < 3000)
                    {
                        angle.verAngle += CHANGE_STEP;//增量为
                        set_shm_angle(shmPtr, angle.horiAngle, angle.verAngle);
                    }    
                  break;
                case 's':
                    printf("DOWN!!!\n");
                    angle.verAngle = shmPtr->tower.veri_angle;//读取TCMP1
                    if(angle.verAngle > CHANGE_STEP)
                    {
                        angle.verAngle -= CHANGE_STEP;//增量为
                        set_shm_angle(shmPtr, angle.horiAngle, angle.verAngle);
                    }
                   break;
                case 'a':   
                    printf("LEFT!!!\n");
                    angle.horiAngle = shmPtr->tower.hori_angle;//读取TCMP1
                    if((angle.horiAngle+CHANGE_STEP) < 3000)
                    {
                        angle.horiAngle += CHANGE_STEP;//增量为
                        set_shm_angle(shmPtr, angle.horiAngle, angle.verAngle);
                    }
                    break;
                case 'd':
                    printf("RIGHT!!!\n");
                    angle.horiAngle = shmPtr->tower.hori_angle;//读取TCMP1
                    if(angle.horiAngle > CHANGE_STEP)
                    {
                        angle.horiAngle -= CHANGE_STEP;//增量为
                        set_shm_angle(shmPtr, angle.horiAngle, angle.verAngle);
                    }
                    break;
                case 'q':
                    printf("Back to pre_menu\n\n");
                    state = INIT_STATE;
                    break;
                default:
                    printf("\nPress \na--right\nd--lef\ns--down\nw--up\n");
                    break;
                }
            }break;
            
            default:
                state = INIT_STATE;
                break;
        }
    }
}

