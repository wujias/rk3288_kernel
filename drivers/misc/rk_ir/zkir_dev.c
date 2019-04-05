#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/poll.h>

#include <linux/gpio.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include <linux/pwm.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/clockchips.h>
#include <linux/delay.h>
#include <linux/percpu.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#ifdef CONFIG_LOCAL_TIMERS
#include <asm/localtimer.h>
#endif
#ifdef CONFIG_ARM
#include <asm/sched_clock.h>
#endif


#define DEVICE_NAME "zkir"

#define SET_DUTY  1111
#define SET_HZ    2222
#define IR_LEARN  3333

#define IRBUFLEN   500
struct pwm_device *pwm = NULL;
static int pwm_status = 0;
static int irhz = 38000;
static int irduty = 30;
static int   ir_count = 0;
static int   ir_len   = 0;
static unsigned int *ir_buf = NULL;
void __iomem *timer3_base = NULL;

static volatile int ev_ircansend = 1; 
static wait_queue_head_t  send_wait;

//ir learn
static struct timer_list captureFail_timer;
static struct timer_list capture_timer;
int ir_learn_irq = 0;
u32 LastCaptureValue = 0, NewCaptureValue = 0;
u32 CaptureValue_us = 0;
u32 * CaptureValue_Table = NULL;
u16 CaptureCount = 0;
u8  CaptureNotFirst = 0, CaptureTimeOutCount = 0;
#define RK_IR_LEARN_GPIO 259
#define CAPTURE_MAX_LEN  1500
#define RK_IR_LEARN_IRQ_TIMER 103

/* 定义并初始化等待队列头*/
static DECLARE_WAIT_QUEUE_HEAD(ir_learn_waitq); 

static volatile int ir_learn_press = 0;

#define TIMERBASE      0xff6b0000
#define RK_IRQ_TIMER   101

#define TIMER_LOAD_COUNT0               0x60
#define TIMER_LOAD_COUNT1               0x64
#define TIMER_CURRENT_VALUE0            0x68
#define TIMER_CURRENT_VALUE1            0x6c
#define TIMER_CONTROL_REG               0x70
#define TIMER_INT_STATUS                0x78


#define TIMER5_LOAD_COUNT0              0xa0
#define TIMER5_LOAD_COUNT1              0xa4
#define TIMER5_CURRENT_VALUE0           0xa8
#define TIMER5_CURRENT_VALUE1           0xac
#define TIMER5_CONTROL_REG              0xb0
#define TIMER5_INT_STATUS               0xb8

#define TIMER_DISABLE                   (0 << 0)
#define TIMER_ENABLE                    (1 << 0)
#define TIMER_MODE_FREE_RUNNING         (0 << 1)
#define TIMER_MODE_USER_DEFINED_COUNT   (1 << 1)
#define TIMER_INT_MASK                  (0 << 2)
#define TIMER_INT_UNMASK                (1 << 2)
#define TIMER_CLEARIRQ                  (1 << 0)

/* wjs add*/
void __iomem *GPIO7_base = NULL;

#define GPIO7BASE                       0xFF7E0000

#define GPIO_SWPORTA_DD                 0x0004
#define GPIO_SWPORTA_DR                 0x0000

#define GPIO7_B1_SET_OUTPUT             (1 << 9)


void __iomem *GPIO8_base = NULL;

#define GPIO8BASE                       0xFF7F0000

#define GPIO8_SWPORTA_DD                 0x0004
#define GPIO8_SWPORTA_DR                 0x0000

#define GPIO8_A3_SET_INPUT              (~(1 << 3))


void __iomem *GRF_base = NULL;

#define GRFBASE                         0xFF770000

#define GRF_GPIO7B_IOMUX                0x0070
#define GRF_GPIO7L_SR                   0x0130
#define GRF_GPIO7B_P                    0x01a4

#define GRF_ENABLE_WRITE_MUX            (3 << 18)
#define GRF_DISABLE_WRITE_MUX           (0 << 18)

#define GRF_ENABLE_WRITE_SR             (1 << 25)
#define GRF_DISABLE_WRITE_SR            (0 << 25)

#define GRF_ENABLE_WRITE_P              (3 << 18)
#define GRF_DISABLE_WRITE_P             (0 << 18)

#define GRF_SET_GPIO7_B1                (0 << 2)
#define GRF_SET_GPIO7_FAST              (1 << 9)
#define GRF_SET_GPIO7_PULL_DOWN         (2 << 2)



#define GRF_GPIO8A_IOMUX                0x0080
#define GRF_GPIO8A_P                    0x01B0
#define GRF_ENABLE_GPIO8_A3_WRITE_MUX   (3 << 22)
#define GRF_ENABLE_WRITE_GPIO8_A3_P     (3 << 22)
#define GRF_SET_GPIO8_A3                (~(3 << 6))
#define GRF_SET_GPIO7_Z                 (~(3 << 6))


static inline void rk_timer_clearirq(void __iomem *base,u32 reg_shift)
{
        writel_relaxed(TIMER_CLEARIRQ, base + reg_shift);
        dsb(sy);
}

static inline void rk_timer_disable(void __iomem *base,u32 reg_shift)
{
        writel_relaxed(TIMER_DISABLE, base + reg_shift);
        dsb(sy);
}

static inline void rk_timer_enable(void __iomem *base, u32 flags,u32 reg_shift)
{
        writel_relaxed(TIMER_ENABLE | flags, base + reg_shift);
        dsb(sy);
}

static inline u64 rk_timer_read_current_value64(void __iomem *base,u32 high_shift,u32 low_shift)
{
        u32 upper, lower;

        do {
                upper = readl_relaxed(base + high_shift);
                lower = readl_relaxed(base + low_shift);
        } while (upper != readl_relaxed(base + high_shift));

        return ((u64) upper << 32) + lower;
}

static inline u64 rk_timer_read_current_value32(void __iomem *base,u32 low_shift)
{
        //u32 lower;
        //lower = readl_relaxed(base + low_shift);
        return readl_relaxed(base + low_shift);
}

static inline void rk_timer_do_set_mode(void __iomem *base,u64 val,u32 high_shift,u32 low_shift)
{      
	    writel_relaxed((u32)val, base + low_shift);
        writel_relaxed((u32)(val>>32), base + high_shift);
        
        dsb(sy);
}

/*wjs add*/
static inline void set_gpio7_funciton_to_gpio(void __iomem *base)
{
    writel_relaxed(GRF_ENABLE_WRITE_MUX, base + GRF_GPIO7B_IOMUX);
    writel_relaxed(GRF_SET_GPIO7_B1, base + GRF_GPIO7B_IOMUX);
    writel_relaxed(GRF_DISABLE_WRITE_MUX, base + GRF_GPIO7B_IOMUX);
    dsb(sy);
}


static inline void set_gpio7_speed_fast(void __iomem *base)
{
    writel_relaxed(GRF_ENABLE_WRITE_SR, base + GRF_GPIO7L_SR);
    writel_relaxed(GRF_SET_GPIO7_FAST, base + GRF_GPIO7L_SR);
    writel_relaxed(GRF_DISABLE_WRITE_SR, base + GRF_GPIO7L_SR);
    dsb(sy);
}

static inline void set_gpio7_pull_down(void __iomem *base)
{
    writel_relaxed(GRF_ENABLE_WRITE_P, base + GRF_GPIO7B_P);
    writel_relaxed(GRF_SET_GPIO7_PULL_DOWN, base + GRF_GPIO7B_P);
    writel_relaxed(GRF_DISABLE_WRITE_P, base + GRF_GPIO7B_P);
    dsb(sy);
}


static inline void rk_set_gpio7_direction_output(void __iomem *base)
{
    u32 temp;
    temp = readl_relaxed(base + GPIO_SWPORTA_DD);
    temp = GPIO7_B1_SET_OUTPUT | temp;
    writel_relaxed(temp, base + GPIO_SWPORTA_DD);
    dsb(sy);
}

static inline void rk_set_gpio_level(void __iomem *base,u32 value)
{
    u32 temp;
    temp = readl_relaxed(base + GPIO_SWPORTA_DR);
    if (value)
       temp = (1 << 9) | temp;
    else
        temp = ~(1 << 9) & temp;
    writel_relaxed(temp, base + GPIO_SWPORTA_DR);
    dsb(sy);
}

static inline void set_gpio_funciton_to_gpio(void __iomem *base,u32 reg_shift,u32 en_val,u32 set_val)
{
    u32 temp;
    temp = readl_relaxed(base + reg_shift);
    temp = (en_val | temp);
    temp = (set_val & temp);
    writel_relaxed(temp, base + reg_shift);
    dsb(sy);
}

static inline void set_gpio_pull_down(void __iomem *base,u32 reg_shift,u32 en_val)
{
    u32 temp;
    temp = readl_relaxed(base + reg_shift);
    temp = (en_val | temp | (1 << 3));
    temp = (~(1 << 2) & temp); 
    writel_relaxed(temp, base + reg_shift);
    dsb(sy);
}

static inline void set_gpio_Z(void __iomem *base,u32 reg_shift,u32 en_val,u32 set_val)
{
    u32 temp;
    temp = readl_relaxed(base + reg_shift);
    temp = (en_val | temp);
    temp = (set_val & temp);
    writel_relaxed(temp, base + reg_shift);
    dsb(sy);
}

static inline void rk_set_gpio_direction_output(void __iomem *base, u32 reg_shift, u32 set_val)
{
    u32 temp;
    temp = readl_relaxed(base + reg_shift);
    temp = (set_val | temp);
    writel_relaxed(temp, base + reg_shift);
    dsb(sy);
}

static inline void rk_set_gpio_direction_input(void __iomem *base, u32 reg_shift, u32 set_val)
{
    u32 temp;
    temp = readl_relaxed(base + reg_shift);
    temp = (set_val & temp);
    writel_relaxed(temp, base + reg_shift);
    dsb(sy);
}

void captureFailtimer_callback(unsigned long arg)
{
    disable_irq_nosync(ir_learn_irq);
    rk_timer_disable(timer3_base,TIMER5_CONTROL_REG);
    del_timer(&capture_timer);
    //ir_learn_press = 1;
    //wake_up_interruptible(&ir_learn_waitq);
}

void capturetimer_callback(unsigned long arg)
{

    disable_irq_nosync(ir_learn_irq);
    rk_timer_disable(timer3_base,TIMER5_CONTROL_REG);
    del_timer(&captureFail_timer);
    ir_learn_press = 1;
    wake_up_interruptible(&ir_learn_waitq);
}

irqreturn_t ir_learn_handler(int irq, void * dev_id)
{
    u8 TimerLoopCount = 0;

    NewCaptureValue = (u32)rk_timer_read_current_value32(timer3_base,TIMER5_CURRENT_VALUE0);

    if (NewCaptureValue < LastCaptureValue)
    {
        CaptureValue_us = (0xFFFFFFFF - LastCaptureValue) + NewCaptureValue;
        if(CaptureTimeOutCount > 0)
            TimerLoopCount = CaptureTimeOutCount - 1;
    }

    else
    {
        CaptureValue_us = NewCaptureValue - LastCaptureValue;
        if(CaptureTimeOutCount > 0)
            TimerLoopCount = CaptureTimeOutCount;
    }

    LastCaptureValue = NewCaptureValue;

    if(CaptureNotFirst)
    {
        if(TimerLoopCount > 0)
            while(TimerLoopCount--)
                CaptureValue_Table[CaptureCount++] = 0xFFFFFFFF;

        CaptureValue_Table[CaptureCount++] = CaptureValue_us;
        CaptureTimeOutCount = 0;
        
        if(CaptureCount >= CAPTURE_MAX_LEN)
            CaptureCount = CAPTURE_MAX_LEN - 1;
    }
    else
    {
        CaptureNotFirst = 1;
        CaptureTimeOutCount = 0;
    }

    mod_timer(&capture_timer,jiffies + 20);

    return IRQ_HANDLED;
}

irqreturn_t mtimer5_irq(int irq, void * dev_id)
{
    rk_timer_clearirq(timer3_base,TIMER5_INT_STATUS);
    CaptureTimeOutCount++;
    return IRQ_HANDLED;
}



irqreturn_t mtimer3_irq(int irq, void * dev_id)
{
    rk_timer_clearirq(timer3_base,TIMER_INT_STATUS);
    rk_timer_disable(timer3_base,TIMER_CONTROL_REG);

    if (ir_count < ir_len)
    {
        if (ir_buf[ir_count -1] != 0xFFFFFFFF)
        { 
            pwm_status = !pwm_status;
        }

        if(ir_buf[ir_count] >= 11000 && ir_buf[ir_count] <= 17000)
        {
            rk_timer_do_set_mode(timer3_base,14000,TIMER_LOAD_COUNT1,TIMER_LOAD_COUNT0);
        }
        else if (ir_buf[ir_count] >= 39000 && ir_buf[ir_count] <= 45000)
        {
            rk_timer_do_set_mode(timer3_base,42000,TIMER_LOAD_COUNT1,TIMER_LOAD_COUNT0);

        }
        else
        {
            rk_timer_do_set_mode(timer3_base,ir_buf[ir_count],TIMER_LOAD_COUNT1,TIMER_LOAD_COUNT0);
        }

        ir_count++;
        rk_set_gpio_level(GPIO7_base,pwm_status);
        rk_timer_enable(timer3_base, TIMER_MODE_USER_DEFINED_COUNT | TIMER_INT_UNMASK,TIMER_CONTROL_REG);
    }
    else
    {

        rk_set_gpio_level(GPIO7_base,0);
        ev_ircansend = 1;
        wake_up_interruptible(&send_wait);
    }

    return IRQ_HANDLED;
}


static int set_pwm_hz(int fz,int n)
{
    int per;
    int ner;
    if(fz == 0)
       return -1;
    per = 1000000000/fz;
    ner = (per*n)/100;        //(1000000000*n)/(fz*100);//n   baifen bi  eg: 10 30 50 90 
    pwm_config(pwm,ner,per);//38KHZ
    return 0;
}


static long zkir_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

    if(cmd != SET_DUTY && cmd != SET_HZ && cmd != IR_LEARN)
    {
        printk("ioctl cmd erro!\n");
        return -1;
    }
    switch(cmd)
    {
/*       
        case SET_DUTY:
            irduty = arg;
            printk("ioctl set irduty!\n");
            break;
        case SET_HZ:
            irhz = arg;
            printk("ioctl set irhz!\n");
            break;
*/
        case IR_LEARN:
            printk("ioctl start ir learn!\n");
            CaptureTimeOutCount = 0;

            del_timer(&captureFail_timer);//
            del_timer(&capture_timer);//

            rk_timer_disable(timer3_base,TIMER5_CONTROL_REG);
            rk_timer_do_set_mode(timer3_base,0xFFFFFFFF,TIMER5_LOAD_COUNT1,TIMER5_LOAD_COUNT0);
            mod_timer(&captureFail_timer,jiffies + 1000);
            enable_irq(ir_learn_irq);
            rk_timer_enable(timer3_base,TIMER_MODE_FREE_RUNNING | TIMER_INT_UNMASK,TIMER5_CONTROL_REG);
            break;
    }

    //pwm_disable(pwm);
    //set_pwm_hz(irhz,irduty);
    //pwm_enable(pwm);    

    return 0;
}


static unsigned int zkir_poll(struct file *file, poll_table *wait)
{  
    unsigned int mask = 0;  
 
    poll_wait(file, &ir_learn_waitq, wait);

    if(ir_learn_press)  
    {  
        mask |= POLLIN | POLLRDNORM;
    }  

    return mask;    
}



static int zkir_open(struct inode *inode, struct file *file)
{

    printk("zkir open\n");
    return 0;
}


static int zkir_close(struct inode *inode, struct file *file)
{

    printk("zkir close\n");
    return 0;

}

static ssize_t zkir_write(struct file *file,const char *buf,size_t count,loff_t *f_pos)
{
    int ret;
    wait_event_interruptible(send_wait,ev_ircansend);
    ev_ircansend = 0;

    if(copy_from_user(ir_buf,buf,count))
    {
        printk("copy_from_user is failed ...\n");
        ret = - EFAULT ;
        ev_ircansend = 1;
        goto out ;
    }
    else
    {
    	ir_len   = count / 4;
    	ir_count = 0;
        ret = count;
        pwm_status = 1;
        rk_timer_disable(timer3_base,TIMER_CONTROL_REG);
        rk_timer_do_set_mode(timer3_base,ir_buf[ir_count++],TIMER_LOAD_COUNT1,TIMER_LOAD_COUNT0);
        rk_set_gpio_level(GPIO7_base,pwm_status);
        rk_timer_enable(timer3_base, TIMER_MODE_USER_DEFINED_COUNT | TIMER_INT_UNMASK,TIMER_CONTROL_REG);
    }
out:
    return ret;
}

static ssize_t zkir_read(struct file *filp,char __user *buffer,size_t size,loff_t * p)
{
    
    u16 tmp;
    u8 *ir_learn_buf = NULL;
    int ret = -1;
    u8 failed_char = 0;

    wait_event_interruptible(ir_learn_waitq, ir_learn_press);
    ir_learn_press = 0;

    if(CaptureCount <= 7 || CaptureCount == (CAPTURE_MAX_LEN - 1))
    {
        CaptureCount = 0;               
        CaptureNotFirst = 0;
        ret = copy_to_user(buffer,&failed_char,1);
        if(ret)
            printk("copy_to_user is failed .....\n");
    }
    else
    {       
        tmp = CaptureCount << 2;
        ir_learn_buf = (u8 *)CaptureValue_Table;
        ret = copy_to_user(buffer,ir_learn_buf,tmp);
        if(ret)
            printk("copy_to_user is failed .....\n");

        CaptureCount = 0;               
        CaptureNotFirst = 0;
    }
    
    return tmp;
}


static struct file_operations zkir_fops=
{
    .owner          =   THIS_MODULE,
    .unlocked_ioctl =   zkir_ioctl,
    .open           =   zkir_open,
    .release        =   zkir_close,
    .write          =   zkir_write,
    .poll           =   zkir_poll,
    .read           =   zkir_read,
};

static struct miscdevice misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = DEVICE_NAME,
    .fops   = &zkir_fops,
};

int __init zkir_init(void)
{
    int ret;

    pwm = pwm_request(2,"ir_pwm");
    if(!pwm)
        goto err1;
        
    printk("pwm request ok!\n");
    
    pwm_disable(pwm);
   
    timer3_base = ioremap(TIMERBASE,SZ_1K);
    if(!timer3_base)
       goto err2;

    //timer3
    rk_timer_clearirq(timer3_base,TIMER_INT_STATUS);
    rk_timer_disable(timer3_base,TIMER_CONTROL_REG);

    //timer5
    rk_timer_clearirq(timer3_base,TIMER5_INT_STATUS);
    rk_timer_disable(timer3_base,TIMER5_CONTROL_REG);

    ret = request_irq(RK_IRQ_TIMER,&mtimer3_irq,IRQF_DISABLED | IRQF_TIMER,"zkir_timer",NULL);//
    if(ret < 0)
       goto err3;

    ret = request_irq(RK_IR_LEARN_IRQ_TIMER,&mtimer5_irq,IRQF_DISABLED | IRQF_TIMER,"zkir_timer5",NULL);//
    if(ret < 0)
      goto err4;

    init_waitqueue_head(&send_wait);

    //wjs add
    GPIO7_base = ioremap(GPIO7BASE,SZ_1K);
    if(!GPIO7_base)
       goto err5;
    GPIO8_base = ioremap(GPIO8BASE,SZ_1K);
    if(!GPIO8_base)
       goto err6;
    GRF_base = ioremap(GRFBASE,SZ_1K);
    if(!GRF_base)
       goto err7;

    //init software timer
    init_timer(&captureFail_timer);                             //初始化内核定时器
    captureFail_timer.function = captureFailtimer_callback;     //指定定时时间到后的回调函数

    init_timer(&capture_timer);                                  //初始化内核定时器
    capture_timer.function = capturetimer_callback;             //指定定时时间到后的回调函数

    set_gpio7_funciton_to_gpio(GRF_base);
    set_gpio7_speed_fast(GRF_base);
    set_gpio7_pull_down(GRF_base);
    rk_set_gpio7_direction_output(GPIO7_base);
    rk_set_gpio_level(GPIO7_base,0);

    //init ir learn gpio
    set_gpio_funciton_to_gpio(GRF_base,GRF_GPIO8A_IOMUX,GRF_ENABLE_GPIO8_A3_WRITE_MUX,GRF_SET_GPIO8_A3);
    set_gpio_Z(GRF_base,GRF_GPIO8A_P,GRF_ENABLE_WRITE_GPIO8_A3_P,GRF_SET_GPIO7_Z);
    rk_set_gpio_direction_input(GPIO8_base,GPIO8_SWPORTA_DR,GPIO8_A3_SET_INPUT);


    //malloc memory
    ir_buf = kmalloc(sizeof(unsigned int)*IRBUFLEN,GFP_KERNEL);
    if(!ir_buf)
    	goto err8;
    CaptureValue_Table = kmalloc(4*CAPTURE_MAX_LEN,GFP_KERNEL);
    if(!CaptureValue_Table)
        goto err9;

    //init ir learn gpio interrupt function
    ir_learn_irq = gpio_to_irq(RK_IR_LEARN_GPIO);
    ret = request_irq(ir_learn_irq,&ir_learn_handler,IRQ_TYPE_EDGE_BOTH,"ir_learn",NULL);
    if(ret < 0)
       goto err10;

    disable_irq_nosync(ir_learn_irq);

    ret = misc_register(&misc);
    if(ret!=0)
    {
        printk("zkir_misc regiter failed\n");
        goto err11;
    }

    printk("the zkir_dev init ok\n");

    set_pwm_hz(irhz,irduty);
    pwm_enable(pwm);

    return 0;

err11:
    free_irq(ir_learn_irq,NULL);
err10:
    kfree(CaptureValue_Table);
err9:
    kfree(ir_buf);
err8:
    iounmap(GRF_base);
    del_timer(&captureFail_timer);
    del_timer(&capture_timer);
err7:
    iounmap(GPIO8_base); 
err6: 
    iounmap(GPIO7_base);
err5:
    free_irq(RK_IR_LEARN_IRQ_TIMER,NULL);      
err4:
    free_irq(RK_IRQ_TIMER,NULL);
err3: 
    iounmap(timer3_base);
err2:
    pwm_free(pwm);;
err1:
    return -1;
}

void __exit zkir_exit(void)
{
   misc_deregister(&misc);
   if(pwm != NULL)
   {
   	    pwm_disable(pwm);
    	pwm_free(pwm);
   }

   free_irq(RK_IRQ_TIMER,NULL);
   free_irq(ir_learn_irq,NULL);
   free_irq(RK_IR_LEARN_IRQ_TIMER,NULL);

   iounmap(timer3_base);
   iounmap(GRF_base);
   iounmap(GPIO7_base);
   iounmap(GPIO8_base);

   del_timer(&captureFail_timer);
   del_timer(&capture_timer);

   kfree(ir_buf);
   kfree(CaptureValue_Table);
   printk("the zkir_dev exit\n");
}

MODULE_AUTHOR("jx") ;
MODULE_LICENSE("GPL");
module_init(zkir_init);
module_exit(zkir_exit);

