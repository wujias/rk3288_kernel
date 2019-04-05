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
#include "RelayIO.h"

#define RELAY_LED_NAME "RELAY_LED"

static struct class *Relay_Led_class;
int led_major;
int relay_gpio;
int bule_led_gpio;
int red_led_gpio;

struct cdev Relay_Led_cdev;

static long Relay_LED_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case RELAY_ON:
			gpio_direction_output(relay_gpio,1);
			break;

		case RELAY_OFF:
			gpio_direction_output(relay_gpio,0);
			break;
			
		case LED0_ON:
			gpio_direction_output(bule_led_gpio,1);
			break;

		case LED0_OFF:
			gpio_direction_output(bule_led_gpio,0);
			break;
			
		case LED1_ON:
			gpio_direction_output(red_led_gpio,1);
			break;

		case LED1_OFF:
			gpio_direction_output(red_led_gpio,0);
			break;
	}

	return 0;
}
/*文件操作结构体*/  
static const struct file_operations Relay_LED_fops =  
{  
	.owner = THIS_MODULE,
	.unlocked_ioctl = Relay_LED_ioctl,
};

static int Relay_LED_probe(struct platform_device *pdev)
{
	int ret = -1;
	dev_t dev;
	enum of_gpio_flags Flag;

	struct device_node *Relay_LED_node = pdev->dev.of_node;

	relay_gpio = of_get_named_gpio_flags(Relay_LED_node, "relay-gpio", 0, &Flag);
	bule_led_gpio = of_get_named_gpio_flags(Relay_LED_node, "bule-gpio", 0, &Flag);
	red_led_gpio = of_get_named_gpio_flags(Relay_LED_node, "red-gpio", 0, &Flag);

	devm_gpio_request(&pdev->dev,relay_gpio,"relay-gpio");
	devm_gpio_request(&pdev->dev,bule_led_gpio,"bule-gpio");
	devm_gpio_request(&pdev->dev,red_led_gpio,"red-gpio");
	

	/*动态分配设备号*/
	ret = alloc_chrdev_region(&dev, 0, 1, "RelayLED");
	if(ret < 0)
	{
		printk("alloc_chrdev is failed ...\n");
		return ret;
	}

	led_major = MAJOR(dev);
	
	/*注册字符设备驱动，设备号和file_operations结构体进行绑定*/
	cdev_init(&Relay_Led_cdev, &Relay_LED_fops);
	Relay_Led_cdev.owner = THIS_MODULE;
	ret = cdev_add(&Relay_Led_cdev, dev, 1);
	if(ret)
	{
		printk("cdev_add is failed ....\n");
		goto ERROR;
	}
	
	/*创建class并将class注册到内核中，返回值为class结构指针*/
	Relay_Led_class = class_create(THIS_MODULE, "Relay");
	device_create(Relay_Led_class, NULL, dev, NULL, "RelayLed");

	return 0;

ERROR:

	devm_gpio_free(&pdev->dev,relay_gpio);
	devm_gpio_free(&pdev->dev,bule_led_gpio);
	devm_gpio_free(&pdev->dev,red_led_gpio);
	
	unregister_chrdev_region(dev,1);

	return -1;
}

static int Relay_LED_remove(struct platform_device *pdev)
{
	devm_gpio_free(&pdev->dev,relay_gpio);
	devm_gpio_free(&pdev->dev,bule_led_gpio);
	devm_gpio_free(&pdev->dev,red_led_gpio);

	device_destroy(Relay_Led_class,MKDEV(led_major, 0));
	class_destroy(Relay_Led_class);

	cdev_del(&Relay_Led_cdev);
	unregister_chrdev_region(MKDEV(led_major, 0), 1);//注销设备

    return 0;
}

static struct platform_device_id Relay_LED_tbl[] = {
    {RELAY_LED_NAME, 0},
    {},
};

static struct of_device_id Relay_LED_dt_ids[] = {
    { .compatible = "rockchip,RelayLed" },
    { }
};

static struct platform_driver Relay_LED_drv = {
    .probe  = Relay_LED_probe,
    .remove = Relay_LED_remove,
	.id_table = Relay_LED_tbl,
    .driver = {
        .name     		= RELAY_LED_NAME,  
        .owner    		= THIS_MODULE,
	    .of_match_table = of_match_ptr(Relay_LED_dt_ids),
    },  
};

static int __init Relay_LED_init(void)
{
	int err = 0;
	err = platform_driver_register(&Relay_LED_drv);

	return err;
}

static void __exit Relay_LED_exit(void)
{
	platform_driver_unregister(&Relay_LED_drv);
}

module_init(Relay_LED_init);
module_exit(Relay_LED_exit);
MODULE_LICENSE ("GPL");