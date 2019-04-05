#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/init.h>  
#include <linux/fs.h>  
#include <linux/cdev.h>  
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>  
#include <asm/io.h>  
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include <linux/irq.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>

#include <linux/workqueue.h>

#include "RelayIO.h"

#define IO_NAME "DRV_IO"
/* 定义并初始化等待队列头*/
static DECLARE_WAIT_QUEUE_HEAD(IO_waitq); 
static struct class *IO_class;
static int IO_major;
static int ev_press = 0;

struct rockchip_IO {
	int 					firstIOvalue;
	int 					secondIOvalue;
	int 					io_gpio;
	int 					dir_gpio;
	unsigned int 			irq;
	struct pinctrl 			*io_pin_ctrl;
	struct pinctrl_state 	*io_on_z;
};

struct rockchip_IO sIOinfo;

struct cdev IO_cdev;

static struct workqueue_struct *IO_workqueue;
static struct work_struct IO_work;


static void work_handler(struct work_struct *work)
{
	mdelay(5);

	sIOinfo.secondIOvalue = gpio_get_value(sIOinfo.io_gpio);
	
	if(sIOinfo.firstIOvalue == sIOinfo.secondIOvalue)
	{
		//3.唤醒等待队列
		ev_press = 1;  
		wake_up_interruptible(&IO_waitq);  
	}
	
	//4.使能中断
	enable_irq(sIOinfo.irq);
}


/* 用户中断处理函数 */
static irqreturn_t IO_irq(int irq, void *dev_id)
{  

	disable_irq_nosync(sIOinfo.irq);

	sIOinfo.firstIOvalue = gpio_get_value(sIOinfo.io_gpio);

	queue_work(IO_workqueue, &IO_work);

    return IRQ_HANDLED;  
}  

static ssize_t IO_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	int ret = -1;
    wait_event_interruptible(IO_waitq, ev_press);
    ev_press = 0;  

    ret = copy_to_user(buf, &sIOinfo.firstIOvalue, sizeof(int)); 
    if (ret)
    {
    	printk("copy_to_user is error......\n");
    	return -1;
    }

    return sizeof(int);   
}

static long IO_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret;

	switch(cmd)
	{

		case SET_IO_TRIG_HIGH:

			free_irq(sIOinfo.irq,NULL);

			flush_workqueue(IO_workqueue);

			gpio_direction_input(sIOinfo.io_gpio);

		    //gpio_direction_output(sIOinfo.dir_gpio,1);

		    //gpio_direction_output(sIOinfo.io_gpio,0);

		    gpio_direction_output(sIOinfo.dir_gpio,0);

	        //ret = gpio_get_value(sIOinfo.io_gpio);

			//printk("first read the GPIO value is [%d].............\n",ret);
			
			ret = request_irq(sIOinfo.irq,IO_irq,IRQ_TYPE_EDGE_RISING,"IO",NULL);
			if (ret)
			{
				printk("request_irq is failed error number is [%d]\n",ret);
				return ret;
			}

			break;

		case SET_IO_TRIG_LOW:

			free_irq(sIOinfo.irq,NULL);

			flush_workqueue(IO_workqueue);

		    gpio_direction_input(sIOinfo.io_gpio);

		    //gpio_direction_output(sIOinfo.dir_gpio,1);

		    //gpio_direction_output(sIOinfo.io_gpio,1);

		    gpio_direction_output(sIOinfo.dir_gpio,0);

	        //ret = gpio_get_value(sIOinfo.io_gpio);

			//printk("first read the GPIO value is [%d].............\n",ret);
			
			ret = request_irq(sIOinfo.irq,IO_irq,IRQ_TYPE_EDGE_FALLING,"IO",NULL);
			if (ret)
			{
				printk("request_irq is failed error number is [%d]\n",ret);
				return ret;
			}
			
			break;
	}

	return 0;
}

static unsigned int IO_poll(struct file *file, poll_table *wait)  
{  
    unsigned int mask = 0;  
 
    poll_wait(file, &IO_waitq, wait);

    if(ev_press)  
    {  
        mask |= POLLIN | POLLRDNORM;
    }  
 
    return mask;    
}

/*文件操作结构体*/  
static const struct file_operations IO_fops =  
{  
	.owner = THIS_MODULE,
	.unlocked_ioctl = IO_ioctl,
	.poll = IO_poll,
	.read = IO_read,
};

static int IO_probe(struct platform_device *pdev)
{
	int ret = -1;
	dev_t dev;
	enum of_gpio_flags Flag;

	struct device_node *IO_node = pdev->dev.of_node;
	sIOinfo.io_gpio = of_get_named_gpio_flags(IO_node, "io-gpio", 0, &Flag);
	sIOinfo.dir_gpio = of_get_named_gpio_flags(IO_node, "dir-gpio", 0, &Flag);
	
	devm_gpio_request(&pdev->dev,sIOinfo.io_gpio,"io-gpio");
	devm_gpio_request(&pdev->dev,sIOinfo.dir_gpio,"dir-gpio");
	
	/* 创建工作队列--workqueue*/
	IO_workqueue = create_workqueue("io_workqueue");
	INIT_WORK(&IO_work, work_handler);

	flush_workqueue(IO_workqueue);
	
	sIOinfo.irq = gpio_to_irq(sIOinfo.io_gpio);

	sIOinfo.io_pin_ctrl = devm_pinctrl_get(&pdev->dev); 

	sIOinfo.io_on_z = pinctrl_lookup_state(sIOinfo.io_pin_ctrl, "default"); 
	
    pinctrl_select_state(sIOinfo.io_pin_ctrl, sIOinfo.io_on_z);

    gpio_direction_input(sIOinfo.io_gpio);


    //gpio_direction_output(sIOinfo.dir_gpio,1);

    //gpio_set_value(sIOinfo.io_gpio,1);

    gpio_direction_output(sIOinfo.dir_gpio,0);

    //ret = gpio_get_value(sIOinfo.io_gpio);

    //printk("first read the GPIO value is [%d].............\n",ret);

	ret = request_irq(sIOinfo.irq,IO_irq,IRQ_TYPE_EDGE_FALLING,"IO",NULL);
	if (ret)
	{
		printk("request_irq is failed error number is [%d]\n",ret);
		return ret;
	}

	/*动态分配设备号*/
	ret = alloc_chrdev_region(&dev, 0, 1, "drv_IO");
	if(ret < 0)
	{
		printk("alloc_chrdev is failed ...\n");
		return ret;
	}

	IO_major = MAJOR(dev);
	
	/*注册字符设备驱动，设备号和file_operations结构体进行绑定*/
	cdev_init(&IO_cdev, &IO_fops);
	IO_cdev.owner = THIS_MODULE;
	ret = cdev_add(&IO_cdev, dev, 1);
	if(ret)
	{
		printk("cdev_add is failed ....\n");
		goto ERROR;
	}
	
	/*创建class并将class注册到内核中，返回值为class结构指针*/
	IO_class = class_create(THIS_MODULE, "CLASSIO");
	device_create(IO_class, NULL, dev, NULL, "IO");

	return 0;

ERROR:
	flush_workqueue(IO_workqueue);
	destroy_workqueue(IO_workqueue);
	
	devm_gpio_free(&pdev->dev,sIOinfo.io_gpio);
	devm_gpio_free(&pdev->dev,sIOinfo.dir_gpio);
	free_irq(sIOinfo.irq,NULL);
	unregister_chrdev_region(dev,1);

	return -1;
}

static int IO_remove(struct platform_device *pdev)
{

	flush_workqueue(IO_workqueue);
	destroy_workqueue(IO_workqueue);
	
	devm_gpio_free(&pdev->dev,sIOinfo.io_gpio);
	devm_gpio_free(&pdev->dev,sIOinfo.dir_gpio);

	device_destroy(IO_class,MKDEV(IO_major, 0));
	class_destroy(IO_class);

	cdev_del(&IO_cdev);
	unregister_chrdev_region(MKDEV(IO_major, 0), 1);//注销设备

    return 0;
}

static struct platform_device_id IO_tbl[] = {
    {IO_NAME, 0},
    {},
};

static struct of_device_id IO_dt_ids[] = {
    { .compatible = "rockchip,IO" },
    { }
};

static struct platform_driver IO_drv = {
    .probe  = IO_probe,
    .remove = IO_remove,
	.id_table = IO_tbl,
    .driver = {
        .name     		= IO_NAME,  
        .owner    		= THIS_MODULE,
	    .of_match_table = of_match_ptr(IO_dt_ids),
    },  
};

static int __init IO_init(void)
{
	int err = 0;
	err = platform_driver_register(&IO_drv);

	return err;
}

static void __exit IO_exit(void)
{
	platform_driver_unregister(&IO_drv);
}

module_init(IO_init);
module_exit(IO_exit);
MODULE_LICENSE ("GPL");