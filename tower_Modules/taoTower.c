/***********************************************************************************
* drivers/char/taoTower.c
* 功能简要： 
*	该驱动注册一个杂项字符设备“/dev/taoTower”, 用于控制舵机。并且通过定时中断控制一个GPIO
*	，以控制cd4052进行通道切换，实现PWM一路分多路．
*   
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

    struct timer_list tower_timer;//定时中断用到的定时器
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

static void timer1_init(void)
{
	unsigned long tcon;//用于存放时钟控制寄存器的数值
    unsigned long tcfg1;//用于存放定时器配置寄存器1的数值
    unsigned long tcfg0;//用于存放定时器配置寄存器0的数值

    /* 读取TCON，TCFG0以及TCFG1寄存器的数值*/
    tcon = __raw_readl(S3C2410_TCON);
    tcfg1 =__raw_readl(S3C2410_TCFG1);
    tcfg0 =__raw_readl(S3C2410_TCFG0);

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
}

static void s3c6410_set_timer1(unsigned long Val)
{
     
	unsigned long tcon;//用于存放时钟控制寄存器的数值
	unsigned long tcnt;//用于存放TCNTB1的数值
	unsigned long tcmp;//用于存放TCMPB1的数值
	tcnt = 0xffffffff;  // 默认的TCTB1数值	
	
	tcon = __raw_readl(S3C2410_TCON);
	//准备更新TCMPB1数值，先置1 TCON[9]
	tcon |= S3C2410_TCON_T1MANUALUPD;//disable manual load,not nessesary.to avorid
	tcon &= ~S3C2410_TCON_T1START; //关闭定时器输出，设置数据
	__raw_writel(tcon, S3C2410_TCON);

	//决定PWM的频率
	tcnt = 20000;
	 __raw_writel(tcnt, S3C2410_TCNTB(1));
	//改变TCMPB1的数值，改数值决定PWM的频宽
    
    tcmp = Val;
	__raw_writel(tcmp, S3C2410_TCMPB(1));

    tower_start();//设置完毕默认启动
}

void time_list_handle(unsigned long none)
{
    unsigned int choice = S_HORIZON;
    unsigned long gpio_data;

	tower_stop();
	__raw_writel(0, S3C2410_TCMPB(1));//避免切换时收到影响,先置pwm输出０

    //在中断中调用下面给GPIO提供的库函数发生BUG: scheduling while atomic
//    choice = (~s3c_gpio_getpin(S3C64XX_GPL(0)))&0x01;//通道切换,把高位掩掉
//    s3c_gpio_setpin(S3C64XX_GPL(0), choice);
    
    gpio_data = __raw_readl(S3C64XX_GPLDAT);

    if(gpio_data&0x01)
    {
	    __raw_writel(gpio_data&(~0x01), S3C64XX_GPLDAT);//通道切换
        s3c6410_set_timer1(towerDev.verticalAngle);
    }
    else
    {
	    __raw_writel(gpio_data | 0x01, S3C64XX_GPLDAT);//通道切换
        s3c6410_set_timer1(towerDev.horizonAngle);
    }

    mod_timer(&towerDev.tower_timer, jiffies + msecs_to_jiffies(REFLASH_TIME_MS));//设定新的值继续加入定时器列表

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

    if(Val>3000)//防止输入脉宽过大
        return 1;

    switch(CMD)
    {
       
        case HORIZON_SET:
            towerDev.horizonAngle = Val;
            break;

        case VERTIAL_SET:
            towerDev.verticalAngle = Val;
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
#define S3C6410_TCMPB0 0 
#define S3C6410_TCMPB1 1
ssize_t tower_read(struct file* file, char __user* buf, size_t count,  loff_t* loff_tt)
{
    //count表示需要读取哪个角度值，１代表水平，０代表竖直

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
    //进行中断定时器初始化
    init_timer(&towerDev.tower_timer);
    towerDev.tower_timer.function = time_list_handle;
    towerDev.tower_timer.expires = jiffies + msecs_to_jiffies(REFLASH_TIME_MS);

    add_timer(&towerDev.tower_timer);//添加定时器

    tower_start();

	return 0;
}
/*关闭设备的接口*/
static int tower_close(struct inode *inode, struct file *file)
{
	tower_stop();
    del_timer(&towerDev.tower_timer);
	return 0;
}

/*接口注册*/
static struct file_operations tower_fops=
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tower_ioctl,
	.open			= tower_open,
	.release 		= tower_close,
    .read           = tower_read,
};

/*设备初始化函数*/
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

/*卸载函数*/
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

