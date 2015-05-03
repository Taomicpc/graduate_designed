/***********************************************************************************
* drivers/char/taoTower.c
* ���ܼ�Ҫ�� 
*	������ע��һ�������ַ��豸��/dev/taoTower��, ���ڿ��ƶ��������ͨ����ʱ�жϿ���һ��GPIO
*	���Կ���cd4052����ͨ���л���ʵ��PWMһ·�ֶ�·��
*   
* ���������ģ��ʱ�����ں�Դ�����Ŀ¼�£�ִ��make SUBDIRS=drivers/char/ modules
*************************************************************************************/
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <mach/map.h>
#include <plat/regs-timer.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <plat/gpio-bank-f.h>
#include <plat/gpio-bank-l.h>
#include <linux/timer.h>

#define DEVICE_NAME "taoTower"

#define PRESCALER (33-1)

#define REFLASH_TIME_MS 200 
#define S3C6410_TCFG1_MUX_DIV2 (1<<4)
#define S3C6410_TCFG1_MUX1_MASK (15<<4)

#define S_HORIZON 1
#define S_VERTICAL 0

struct towerDever
{
    struct miscdevice towerMiscdevi;
 
    unsigned long horizonAngle;
    unsigned long verticalAngle;

    struct timer_list tower_timer;//��ʱ�ж��õ��Ķ�ʱ��
}towerDev;

static void tower_start(void);
static void tower_stop(void);
static void timer1_init(void);
static void s3c6410_set_timer1(unsigned long Val);
static void time_list_handle(unsigned long none);
static long tower_ioctl(struct file *file, unsigned int CMD, unsigned long Val);
ssize_t tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt);
static int tower_open(struct inode *inode, struct file *file);
static int tower_close(struct inode *inode, struct file *file);
static int tower_init(void);
static void tower_exit(void);

/*
*�����������趨�ö�ʱ��1��PWMģʽ��
*��ʼ��Ч��ʱ��1
*/
static void tower_start(void)
{
	unsigned long tcon;

	tcon = __raw_readl(S3C2410_TCON);//��ȡʱ�ӿ��ƼĴ���
	tcon &= ~S3C2410_TCON_T1MANUALUPD;//֮ǰ�Ѿ����ú���TCNTB1 �Լ� TCMPB1�����������κβ���
	tcon |= S3C2410_TCON_T1START;//��ʼλ��1,�ö�ʱ����ʼ����
	__raw_writel(tcon, S3C2410_TCON);//������д�ؼĴ���
}

static void tower_stop(void)
{
	unsigned long tcon;
	//��GPF15��������Ϊ����
	//s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_INPUT);
	//����ʱ�����ƼĴ����е�TIMER1��ʼλ����Ϊ0
	tcon = __raw_readl(S3C2410_TCON);
	tcon &= ~S3C2410_TCON_T1START;
	tcon &= ~S3C2410_TCON_T1MANUALUPD;//ֹͣ���¹���
	__raw_writel(tcon, S3C2410_TCON); //stop the timer1
}

static void timer1_init(void)
{
	unsigned long tcon;//���ڴ��ʱ�ӿ��ƼĴ�������ֵ
    unsigned long tcfg1;//���ڴ�Ŷ�ʱ�����üĴ���1����ֵ
    unsigned long tcfg0;//���ڴ�Ŷ�ʱ�����üĴ���0����ֵ

    /* ��ȡTCON��TCFG0�Լ�TCFG1�Ĵ�������ֵ*/
    tcon = __raw_readl(S3C2410_TCON);
    tcfg1 =__raw_readl(S3C2410_TCFG1);
    tcfg0 =__raw_readl(S3C2410_TCFG0);

	/*��ʱ������Ƶ�� = PCLK / ( {Ԥ��Ƶ��ֵ + 1} ) / {�ָ���ֵ}
	*{Ԥ��Ƶ��ֵ} = 1~255����TCFG0���üĴ���������
	*{�ָ���ֵ} = 1, 2, 4, 8, 16, TCLK����TCFG1���üĴ���������
	*/
      
	//����GPF15Ϊ���
	s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_PWM_TOUT1);
    //����ͨ���л��ܽ�
    s3c_gpio_cfgpin(S3C64XX_GPL(0),S3C64XX_GPL_OUTPUT(0));
	//����TCFG1[4:7]
    tcfg1 &= ~S3C6410_TCFG1_MUX1_MASK;
	//���÷ָ�ֵΪ2
    tcfg1 |= S3C6410_TCFG1_MUX_DIV2;//set [4:7]== 1/2

	//����Ԥ��ƵλTCFG0[0:7]
    tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;//�˴��Ѳ�6410��2410����
	//����Ԥ��Ƶ��ֵ��������254
    tcfg0 |= (PRESCALER) << S3C6410_TCFG_PRESCALER0_SHIFT;//�˴��Ѳ�6410��2410

	//�����úõ�TCON��TCFG0��TCFG1����ֵд�ؼĴ���
	__raw_writel(tcfg1, S3C2410_TCFG1);
	__raw_writel(tcfg0, S3C2410_TCFG0);
	
	//����TCON[8:10]
    tcon &= ~(7<<8); //set bit [8:10] to zero
	//���ö�ʱ������ģʽΪ�Զ�����ģʽ(auto-reload)
    tcon |= S3C2410_TCON_T1RELOAD;

	__raw_writel(tcon, S3C2410_TCON);
}

static void s3c6410_set_timer1(unsigned long Val)
{
     
	unsigned long tcon;//���ڴ��ʱ�ӿ��ƼĴ�������ֵ
	unsigned long tcnt;//���ڴ��TCNTB1����ֵ
	unsigned long tcmp;//���ڴ��TCMPB1����ֵ
	tcnt = 0xffffffff;  // Ĭ�ϵ�TCTB1��ֵ	
	
	tcon = __raw_readl(S3C2410_TCON);
	//׼������TCMPB1��ֵ������1 TCON[9]
	tcon |= S3C2410_TCON_T1MANUALUPD;//disable manual load,not nessesary.to avorid
	tcon &= ~S3C2410_TCON_T1START; //�رն�ʱ���������������
	__raw_writel(tcon, S3C2410_TCON);

	//����PWM��Ƶ��
	tcnt = 20000;
	 __raw_writel(tcnt, S3C2410_TCNTB(1));
	//�ı�TCMPB1����ֵ������ֵ����PWM��Ƶ��
    
    tcmp = Val;
	__raw_writel(tcmp, S3C2410_TCMPB(1));

    tower_start();//�������Ĭ������
}

void time_list_handle(unsigned long none)
{
    unsigned int choice = S_HORIZON;
    unsigned long gpio_data;

	tower_stop();
	__raw_writel(0, S3C2410_TCMPB(1));//�����л�ʱ�յ�Ӱ��,����pwm�����

    //���ж��е��������GPIO�ṩ�Ŀ⺯������BUG: scheduling while atomic
//    choice = (~s3c_gpio_getpin(S3C64XX_GPL(0)))&0x01;//ͨ���л�,�Ѹ�λ�ڵ�
//    s3c_gpio_setpin(S3C64XX_GPL(0), choice);
    
    gpio_data = __raw_readl(S3C64XX_GPLDAT);

    if(gpio_data&0x01)
    {
	    __raw_writel(gpio_data&(~0x01), S3C64XX_GPLDAT);//ͨ���л�
        s3c6410_set_timer1(towerDev.verticalAngle);
    }
    else
    {
	    __raw_writel(gpio_data | 0x01, S3C64XX_GPLDAT);//ͨ���л�
        s3c6410_set_timer1(towerDev.horizonAngle);
    }

    mod_timer(&towerDev.tower_timer, jiffies + msecs_to_jiffies(REFLASH_TIME_MS));//�趨�µ�ֵ�������붨ʱ���б�

}

#define HORIZON (0x1<<31)
#define VERTIAL (0x0)

#define HORIZON_ON (HORIZON | 0x1)
#define HORIZON_OFF (HORIZON & (~0x1))
#define HORIZON_SET (HORIZON_ON | (0x1<<1))

#define VERTIAL_ON (VERTIAL | 0x1)
#define VERTIAL_OFF (VERTIAL & (~0x1))
#define VERTIAL_SET (VERTIAL_ON | (0x1<<1))

static long tower_ioctl(
    struct file *file,
    unsigned int CMD,
    unsigned long Val)
{

    if(Val>3000)//��ֹ�����������
        return 1;

    switch(CMD)
    {
       
        case HORIZON_SET:
            towerDev.horizonAngle = Val;
            break;

        case VERTIAL_SET:
            towerDev.verticalAngle = Val;
            break;

        case VERTIAL_OFF://����ֹͣ��ˮƽ����ֱ��������
        case HORIZON_OFF:
            tower_stop();
            break;
   
        default:
            break;
    }

	return 0;
}

//���Ĵ�����ֵ
#define S3C6410_TCMPB0 0 
#define S3C6410_TCMPB1 1
ssize_t tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt)
{
    //count��ʾ��Ҫ��ȡ�ĸ��Ƕ�ֵ��������ˮƽ����������ֱ

    if(count == S_HORIZON)
    {
        put_user( towerDev.horizonAngle, (unsigned long*)buf );
        return 4;
    }
    else if(count == S_VERTICAL)
    {
        put_user( towerDev.verticalAngle, (unsigned long*)buf );
        return 4;
    }
    else 
        return 0;
}

//when open beep device, this function will be called
static int tower_open(struct inode *inode, struct file *file)
{
    //�����ж϶�ʱ����ʼ��
    init_timer(&towerDev.tower_timer);
    towerDev.tower_timer.function = time_list_handle;
    towerDev.tower_timer.expires = jiffies + msecs_to_jiffies(REFLASH_TIME_MS);

    add_timer(&towerDev.tower_timer);//��Ӷ�ʱ��

    tower_start();

	return 0;
}
/*�ر��豸�Ľӿ�*/
static int tower_close(struct inode *inode, struct file *file)
{
	tower_stop();
    del_timer(&towerDev.tower_timer);
	return 0;
}

/*�ӿ�ע��*/
static struct file_operations tower_fops=
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tower_ioctl,
	.open			= tower_open,
	.release 		= tower_close,
    .read           = tower_read,
};

/*�豸��ʼ������*/
static int tower_init(void)
{
	int ret;
    
    towerDev.towerMiscdevi.minor = MISC_DYNAMIC_MINOR;
    towerDev.towerMiscdevi.name	= DEVICE_NAME;
    towerDev.towerMiscdevi.fops	= &tower_fops;
    towerDev.horizonAngle = 0;
    towerDev.verticalAngle = 0;

	ret = misc_register(&towerDev.towerMiscdevi);

	if(ret<0)
	{
		printk(DEVICE_NAME "can't register major number\n");
		return ret;
	}
	timer1_init();
	tower_stop();
	printk(KERN_INFO "Tower driver successfully probed\n");
	return 0;
}

/*ж�غ���*/
static void tower_exit(void)
{
	misc_deregister(&towerDev.towerMiscdevi);
    
    printk("Goodbye Tower module !\n");
} 

module_init(tower_init);
module_exit(tower_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tao");
MODULE_DESCRIPTION("PWM based tower Driver");

