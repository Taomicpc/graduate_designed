/***********************************************************************************
* drivers/char/taotower.c
* �˰汾ͨ��TIMER������Ķ�ʱ�������жϣ�����ͨ���л�.
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
#include <linux/interrupt.h>

#define DEVICE_NAME "taoTower"
static DECLARE_WAIT_QUEUE_HEAD(timer1_wait);

#define PRESCALER (33-1)


//#define REFLASH_TIME_MS 80 

struct towerDever
{
    struct miscdevice towerMiscdevi;
 
    unsigned long horizonAngle;
    unsigned long vertialAngle;

    struct timer_list tower_timer;//��ʱ�ж��õ��Ķ�ʱ��
}towerDev;

static void tower_start(void);
static void tower_stop(void);
static void timer1_init(void);
static void s3c6410_set_timer1(unsigned long Val);
static irqreturn_t timer1_handle(int none, void * nothing);
static long tq6410_beep_ioctl(struct file *file, unsigned int CMD, unsigned long Val);
ssize_t tq6410_tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt);
static int tq6410_beep_open(struct inode *inode, struct file *file);
static int tq6410_adc_close(struct inode *inode, struct file *file);
static int tq6410_beep_init(void);
static void tq6410_beep_exit(void);

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
//    s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_PWM_TOUT1);//pwm����ܽ�
//    s3c_gpio_cfgpin(S3C64XX_GPL(0),S3C64XX_GPL_OUTPUT(0));//ͨ���л��ܽ�
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

#define S3C6410_TCFG1_MUX_DIV2 (1<<4)
#define S3C6410_TCFG1_MUX1_MASK (15<<4)
static void timer1_init(void)
{
	unsigned long tcon;//���ڴ��ʱ�ӿ��ƼĴ�������ֵ
    unsigned long tcfg1;//���ڴ�Ŷ�ʱ�����üĴ���1����ֵ
    unsigned long tcfg0;//���ڴ�Ŷ�ʱ�����üĴ���0����ֵ
    unsigned long tint;//���ڴ�Ŷ�ʱ���ж���Ϣ����ֵ

        /* ��ȡTCON��TCFG0�Լ�TCFG1�Ĵ�������ֵ*/
    tcon = __raw_readl(S3C2410_TCON);
    tcfg1 =__raw_readl(S3C2410_TCFG1);
    tcfg0 =__raw_readl(S3C2410_TCFG0);
    tint =__raw_readl(S3C64XX_TINT_CSTAT);

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


    //����TIMER1���ж�
        tint |= 0x42;//�üĴ���bit1Ϊtimer1�ж�ʧ�ܣ�bit6Ϊtimer1�ж�״̬λ
    
	__raw_writel(tint, S3C64XX_TINT_CSTAT);

}

#define S_HORIZON 1
#define S_VERTICAL 0

static void s3c6410_set_timer1(unsigned long Val)
{
     
	unsigned long tcon;//���ڴ��ʱ�ӿ��ƼĴ�������ֵ
	unsigned long tcnt;//���ڴ��TCNTB1����ֵ
	unsigned long tcmp;//���ڴ��TCMPB1����ֵ
	tcnt = 0xffffffff;  // Ĭ�ϵ�TCTB1��ֵ	
	
	tcon = __raw_readl(S3C2410_TCON);
	//׼������TCMPB1��ֵ������1 TCON[9]
	tcon |= S3C2410_TCON_T1MANUALUPD;//disable manual load,not nessesary.to avorid
	__raw_writel(tcon, S3C2410_TCON);

	//����PWM��Ƶ��
	tcnt = 20000;
	 __raw_writel(tcnt, S3C2410_TCNTB(1));
	//�ı�TCMPB1����ֵ������ֵ����PWM��Ƶ��
    
    tcmp = Val;
	__raw_writel(tcmp, S3C2410_TCMPB(1));

    tower_start();//�������Ĭ������
}

//�����������׼���жϺ���ģʽ
irqreturn_t timer1_handle(int none, void* nothing)
{
    unsigned int choice = S_HORIZON;
    unsigned long tint;

    tint =__raw_readl(S3C64XX_TINT_CSTAT);
    tint |= (0x1<<6);//���timer1�����ж�

    //printk("timer1 interrupt!\n");

    udelay(5);
	//__raw_writel(0, S3C2410_TCMPB(1));//�����л�ʱ�յ�Ӱ��,����pwm�����
    s3c6410_set_timer1(0);
    
    choice = (~s3c_gpio_getpin(S3C64XX_GPL(0)))&0x01;//ͨ���л�,�Ѹ�λ�ڵ�
    s3c_gpio_setpin(S3C64XX_GPL(0), choice);
    
    if(choice == S_HORIZON)
        s3c6410_set_timer1(towerDev.horizonAngle);
    else if(choice == S_VERTICAL)
        s3c6410_set_timer1(towerDev.vertialAngle);

    return IRQ_HANDLED;
}

/*
*���������ڸ��¶�ʱ��1��TCTB1�Լ�TCMPB1����ֵ��
*ͨ������
*/
#define HORIZON (0x1<<31)
#define VERTIAL (0x0)

#define HORIZON_ON (HORIZON | 0x1)
#define HORIZON_OFF (HORIZON & (~0x1))
#define HORIZON_SET (HORIZON_ON | (0x1<<1))

#define VERTIAL_ON (VERTIAL | 0x1)
#define VERTIAL_OFF (VERTIAL & (~0x1))
#define VERTIAL_SET (VERTIAL_ON | (0x1<<1))

static long tq6410_beep_ioctl(
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
            towerDev.vertialAngle = Val;
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
//
#define S3C6410_TCMPB0 0 
#define S3C6410_TCMPB1 1

ssize_t tq6410_tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt)
{
    //count��ʾ��Ҫ��ȡ�ĸ��Ƕ�ֵ��������ˮƽ����������ֱ

    if(count == S_HORIZON)
    {
        put_user( towerDev.horizonAngle, (unsigned long*)buf );
        return 4;
    }
    else if(count == S_VERTICAL)
    {
        put_user( towerDev.vertialAngle, (unsigned long*)buf );
        return 4;
    }
    else 
        return 0;
}

//when open beep device, this function will be called
static int tq6410_beep_open(struct inode *inode, struct file *file)
{
    //���ж�ʱ����ʼ��
    int ret;

    s3c6410_set_timer1(1000);//��ʼ��Ϊ1000

    ret = request_irq(IRQ_TIMER1, &timer1_handle, IRQF_SHARED, "timer1_update_irq", NULL);//ע���ж�
//    ret = request_irq(IRQ_TIMER1, &timer1_handle, IRQF_DISABLED, "timer1_update_irq", NULL);//ע���ж�
    
    if(ret)
    {
        printk("request_irq timer1 error\n");
    }

    tower_start();

#ifdef CONFIG_TQ6410_DEBUG_BEEP
	printk(KERN_INFO " beep opened\n");
#endif
	return 0;

}
/*�ر��豸�Ľӿ�*/
static int tq6410_adc_close(struct inode *inode, struct file *file)
{
    tower_stop();
    free_irq(IRQ_TIMER1, NULL);//�ͷ��жϺ�
	return 0;
}

/*�ӿ�ע��*/
static struct file_operations tower_fops=
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tq6410_beep_ioctl,
	.open			= tq6410_beep_open,
	.release 		= tq6410_adc_close,
    .read           = tq6410_tower_read,
};

/*�豸��ʼ������*/
static int tq6410_beep_init(void)
{
	int ret;
    
    towerDev.towerMiscdevi.minor = MISC_DYNAMIC_MINOR;
    towerDev.towerMiscdevi.name	= DEVICE_NAME;
    towerDev.towerMiscdevi.fops	= &tower_fops;
    towerDev.horizonAngle = 0;
    towerDev.vertialAngle = 0;

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
static void tq6410_beep_exit(void)
{
	misc_deregister(&towerDev.towerMiscdevi);
    
    printk("Goodbye Tower module !\n");
} 



module_init(tq6410_beep_init);
module_exit(tq6410_beep_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tao");
MODULE_DESCRIPTION("PWM based tower Driver");
