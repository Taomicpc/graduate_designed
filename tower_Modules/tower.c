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
    char choice;
    int state = INIT_STATE;
    
    struct Angle
    {
        unsigned long horiAngle ;
        unsigned long verAngle ;
    }angle;
    
    int fd=3;
    int pwm=0;
    int i;

    fd=open("/dev/taoTower",O_RDWR);
    if(fd<0)
    {
        printf("API join error");
        exit(1);
    }

    ioctl(fd, HORIZON_SET, 1000);     
    ioctl(fd, VERTIAL_SET, 1000);     

    printf("\nHello!Welcome to tao's tower control!\n");
    printf("you could input how you want to control\n");

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
                choice = myGetchar();//非阻塞获取
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
                        ioctl(fd, HORIZON_OFF, 0);     
                        close(fd);
                        exit(0);
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
                    ioctl(fd, HORIZON_SET, angle.horiAngle);     
                    ioctl(fd, VERTIAL_SET, angle.verAngle);     
                    //printf("Right input!set angle %d and %d\n", angle.horiAngle, angle.verAngle);
                }
            }break;

            case CTL_ANGLE_STATE:
            {//a,s,d,w上下左右控制状态
                choice = myGetchar();
                switch(choice)
                {
                case 'w':
                    printf("UP!!!\n");
                    read(fd, &angle.verAngle, S_VERTICAL);//读取TCMP1
                    if((angle.verAngle+CHANGE_STEP) < 3000)
                    {
                        angle.verAngle += CHANGE_STEP;//增量为
                        ioctl(fd, VERTIAL_SET, angle.verAngle);
                    }    
                  break;
                case 's':
                    printf("DOWN!!!\n");
                    read(fd, &angle.verAngle, S_VERTICAL);//读取TCMP1
                    if(angle.verAngle > CHANGE_STEP)
                    {
                        angle.verAngle -= CHANGE_STEP;//增量为
                        ioctl(fd, VERTIAL_SET, angle.verAngle);     
                    }
                   break;
                case 'a':   
                    printf("LEFT!!!\n");
                    read(fd, &angle.horiAngle, S_HORIZON);//读取TCMP1
                    if((angle.horiAngle+CHANGE_STEP) < 3000)
                    {
                        angle.horiAngle += CHANGE_STEP;//增量为
                        ioctl(fd, HORIZON_SET, angle.horiAngle);     
                    }
                    break;
                case 'd':
                    printf("RIGHT!!!\n");
                    read(fd, &angle.horiAngle, S_HORIZON);//读取TCMP1
                    if(angle.horiAngle > CHANGE_STEP)
                    {
                        angle.horiAngle -= CHANGE_STEP;//增量为
                        ioctl(fd, HORIZON_SET, angle.horiAngle);
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
    close(fd);
    return 0;
}

