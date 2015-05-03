/***********************************************************************************
* drivers/char/taotower.c
* 此版本通过TIMER１自身的定时器更新中断，进行通道切换.
* 单独编译此模块时，在内核源程序根目录下，执行make SUBDIRS=drivers/char/ modules
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

    struct timer_list tower_timer;//定时中断用到的定时器
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
*函数用于在设定好定时器1的PWM模式后，
*开始生效定时器1
*/
static void tower_start(void)
{
	unsigned long tcon;

	tcon = __raw_readl(S3C2410_TCON);//读取时钟控制寄存器
	tcon &= ~S3C2410_TCON_T1MANUALUPD;//之前已经设置好了TCNTB1 以及 TCMPB1，这里无须任何操作
	tcon |= S3C2410_TCON_T1START;//开始位置1,让定时器开始工作
	__raw_writel(tcon, S3C2410_TCON);//将设置写回寄存器
//    s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_PWM_TOUT1);//pwm输出管脚
//    s3c_gpio_cfgpin(S3C64XX_GPL(0),S3C64XX_GPL_OUTPUT(0));//通道切换管脚
}

static void tower_stop(void)
{
	unsigned long tcon;
	//将GPF15引脚设置为输入
	//s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_INPUT);
	//将定时器控制寄存器中的TIMER1开始位设置为0
	tcon = __raw_readl(S3C2410_TCON);
	tcon &= ~S3C2410_TCON_T1START;
	tcon &= ~S3C2410_TCON_T1MANUALUPD;//停止更新功能
	__raw_writel(tcon, S3C2410_TCON); //stop the timer1
}

#define S3C6410_TCFG1_MUX_DIV2 (1<<4)
#define S3C6410_TCFG1_MUX1_MASK (15<<4)
static void timer1_init(void)
{
	unsigned long tcon;//用于存放时钟控制寄存器的数值
    unsigned long tcfg1;//用于存放定时器配置寄存器1的数值
    unsigned long tcfg0;//用于存放定时器配置寄存器0的数值
    unsigned long tint;//用于存放定时器中断信息的数值

        /* 读取TCON，TCFG0以及TCFG1寄存器的数值*/
    tcon = __raw_readl(S3C2410_TCON);
    tcfg1 =__raw_readl(S3C2410_TCFG1);
    tcfg0 =__raw_readl(S3C2410_TCFG0);
    tint =__raw_readl(S3C64XX_TINT_CSTAT);

	/*定时器输入频率 = PCLK / ( {预分频数值 + 1} ) / {分割数值}
	*{预分频数值} = 1~255，由TCFG0配置寄存器来配置
	*{分割数值} = 1, 2, 4, 8, 16, TCLK，由TCFG1配置寄存器来配置
	*/
      
	//配置GPF15为输出
	s3c_gpio_cfgpin(S3C64XX_GPF(15),S3C64XX_GPF15_PWM_TOUT1);
    //配置通道切换管脚
    s3c_gpio_cfgpin(S3C64XX_GPL(0),S3C64XX_GPL_OUTPUT(0));
	//清零TCFG1[4:7]
        tcfg1 &= ~S3C6410_TCFG1_MUX1_MASK;
	//设置分割值为2
        tcfg1 |= S3C6410_TCFG1_MUX_DIV2;//set [4:7]== 1/2

	//清零预分频位TCFG0[0:7]
        tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;//此处已查6410与2410兼容
	//设置预分频数值，这里是254
        tcfg0 |= (PRESCALER) << S3C6410_TCFG_PRESCALER0_SHIFT;//此处已查6410与2410

	//将配置好的TCON，TCFG0，TCFG1的数值写回寄存器
	__raw_writel(tcfg1, S3C2410_TCFG1);
	__raw_writel(tcfg0, S3C2410_TCFG0);
	
	//清零TCON[8:10]
        tcon &= ~(7<<8); //set bit [8:10] to zero
	//设置定时器工作模式为自动加载模式(auto-reload)
        tcon |= S3C2410_TCON_T1RELOAD;

	__raw_writel(tcon, S3C2410_TCON);


    //开启TIMER1的中断
        tint |= 0x42;//该寄存器bit1为timer1中断失能，bit6为timer1中断状态位
    
	__raw_writel(tint, S3C64XX_TINT_CSTAT);

}

#define S_HORIZON 1
#define S_VERTICAL 0

static void s3c6410_set_timer1(unsigned long Val)
{
     
	unsigned long tcon;//用于存放时钟控制寄存器的数值
	unsigned long tcnt;//用于存放TCNTB1的数值
	unsigned long tcmp;//用于存放TCMPB1的数值
	tcnt = 0xffffffff;  // 默认的TCTB1数值	
	
	tcon = __raw_readl(S3C2410_TCON);
	//准备更新TCMPB1数值，先置1 TCON[9]
	tcon |= S3C2410_TCON_T1MANUALUPD;//disable manual load,not nessesary.to avorid
	__raw_writel(tcon, S3C2410_TCON);

	//决定PWM的频率
	tcnt = 20000;
	 __raw_writel(tcnt, S3C2410_TCNTB(1));
	//改变TCMPB1的数值，改数值决定PWM的频宽
    
    tcmp = Val;
	__raw_writel(tcmp, S3C2410_TCMPB(1));

    tower_start();//设置完毕默认启动
}

//输入输出按标准的中断函数模式
irqreturn_t timer1_handle(int none, void* nothing)
{
    unsigned int choice = S_HORIZON;
    unsigned long tint;

    tint =__raw_readl(S3C64XX_TINT_CSTAT);
    tint |= (0x1<<6);//清除timer1更新中断

    //printk("timer1 interrupt!\n");

    udelay(5);
	//__raw_writel(0, S3C2410_TCMPB(1));//避免切换时收到影响,先置pwm输出０
    s3c6410_set_timer1(0);
    
    choice = (~s3c_gpio_getpin(S3C64XX_GPL(0)))&0x01;//通道切换,把高位掩掉
    s3c_gpio_setpin(S3C64XX_GPL(0), choice);
    
    if(choice == S_HORIZON)
        s3c6410_set_timer1(towerDev.horizonAngle);
    else if(choice == S_VERTICAL)
        s3c6410_set_timer1(towerDev.vertialAngle);

    return IRQ_HANDLED;
}

/*
*函数用于在更新定时器1的TCTB1以及TCMPB1的数值，
*通过更新
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

    if(Val>3000)//防止输入脉宽过大
        return 1;

    switch(CMD)
    {
       
        case HORIZON_SET:
            towerDev.horizonAngle = Val;
            break;

        case VERTIAL_SET:
            towerDev.vertialAngle = Val;
            break;

        case VERTIAL_OFF://对于停止，水平和竖直不做区分
        case HORIZON_OFF:
            tower_stop();
            break;
   
        default:
            break;
    }

	return 0;
}

//读寄存器的值
//
#define S3C6410_TCMPB0 0 
#define S3C6410_TCMPB1 1

ssize_t tq6410_tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt)
{
    //count表示需要读取哪个角度值，１代表水平，０代表竖直

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
    //进行定时器初始化
    int ret;

    s3c6410_set_timer1(1000);//初始化为1000

    ret = request_irq(IRQ_TIMER1, &timer1_handle, IRQF_SHARED, "timer1_update_irq", NULL);//注册中断
//    ret = request_irq(IRQ_TIMER1, &timer1_handle, IRQF_DISABLED, "timer1_update_irq", NULL);//注册中断
    
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
/*关闭设备的接口*/
static int tq6410_adc_close(struct inode *inode, struct file *file)
{
    tower_stop();
    free_irq(IRQ_TIMER1, NULL);//释放中断号
	return 0;
}

/*接口注册*/
static struct file_operations tower_fops=
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tq6410_beep_ioctl,
	.open			= tq6410_beep_open,
	.release 		= tq6410_adc_close,
    .read           = tq6410_tower_read,
};

/*设备初始化函数*/
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

/*卸载函数*/
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
