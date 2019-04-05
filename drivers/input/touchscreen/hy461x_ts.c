/*
 * drivers/input/touchscreen/hy461x_ts.c
 *
 * HY hy461x TouchScreen driver.
 *
 * Copyright (c) 2012 FriendlyARM (www.arm9.net)
 *
 * Copyright (c) 2010 Focal tech Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>

#undef CONFIG_FB
#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>
#elif defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif

//#include <linux/input/hy461x_ts.h>
#include "hy461x_ts.h"
#define TP_DBG_ON  0
#define TP_DBG(fmt, args...)       do{   if(TP_DBG_ON) printk(KERN_ERR pr_fmt(fmt), ##args); }while(0)

/* NOTE: support mulititouch only */
#define CONFIG_HY461X_MULTITOUCH		1

//#define HY_UPDATE_FW_ONLINE

static int gpio_reset;
/*---------------------------------------------------------
 * Chip I/O operations
 */

static struct i2c_client *this_client;

static bool i2c_read_interface(u8* pbt_buf, int dw_lenth)
{
    int ret;    
    ret=i2c_master_recv(this_client, pbt_buf, dw_lenth);
    if(ret<=0)
    {
        printk("[TSP]i2c_read_interface error\n");
        return 0;
    }  
    return 1;
}
static bool  i2c_write_interface(u8* pbt_buf, int dw_lenth)
{
    int ret;
    ret=i2c_master_send(this_client, pbt_buf, dw_lenth);
    if(ret<0)
    {
        printk("[TSP]i2c_write_interface error line = %d, ret = %d\n", __LINE__, ret);
        return 0;
    }
    return 1;
}

static int hy461x_i2c_rxdata(char *rxdata, int length) {
	int ret;
	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= rxdata,
            .scl_rate = 400000,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
            .scl_rate = 400000,
		},
	};

	ret = i2c_transfer(this_client->adapter, msgs, 2);
	if (ret < 0)
		pr_err("%s: i2c read error: %d\n", __func__, ret);

	return ret;
}

static int hy461x_read_reg(u8 addr, u8 *rx_buf, int rx_length)
{
	u8 read_cmd[2]= {0};
	u8 cmd_len 	= 0;

	read_cmd[0] = addr;
	cmd_len = 1;	

	/*send register addr*/
	if(!i2c_write_interface(&read_cmd[0], cmd_len))
	{
		return 0;
	}

	/*call the read callback function to get the register value*/		
	if(!i2c_read_interface(rx_buf, rx_length))
	{
		return 0;
	}
	return 1;
}
static int hy461x_write_reg(u8 addr, u8 *pdata)
 {
	u8 write_reg[2] = {0};
	write_reg[0] = addr;
	write_reg[1] = *pdata;
	return i2c_write_interface(write_reg, 2);		
}
static int hy461x_read_fw_ver(unsigned char *val)
{
	int ret;	
	u8 normode = 0x00;//PMODE_ACTIVE
	*val = 0xff;;
	if (!hy461x_write_reg(HY461X_REG_MODE, &normode))
	printk("Switch Normal mode faild@@!!\n");

	ret = hy461x_read_reg(HY461X_REG_FIRMID, val, 1);

	return ret;
}

/*---------------------------------------------------------
 * Touch core support
 */

static void hy461x_ts_report(struct hy461x_ts_data *ts) {
	struct hy461x_event *event = &ts->event;
	int i;

	for (i = 0; i < event->touch_point; i++) 
	//for (i = 0; i< 1; i++)
	{			
		input_report_abs(ts->input_dev, ABS_X, event->x[i]);
		input_report_abs(ts->input_dev, ABS_Y, ts->pdata->screen_max_y - event->y[i]);
		if(event->au8_touch_event[i] == HYS_POINT_DOWN || event->au8_touch_event[i] == HYS_POINT_CONTACT)
		{	
			input_report_abs(ts->input_dev, ABS_PRESSURE, 200);
            input_report_key(ts->input_dev, BTN_TOUCH, 1);
		}	
		else
		{
            input_report_key(ts->input_dev, BTN_TOUCH, 0);
			input_report_abs(ts->input_dev, ABS_PRESSURE, 0);
		}

	}

	input_sync(ts->input_dev);
}
/*
static void hy461x_ts_release(struct hy461x_ts_data *ts) 
{
	input_sync(ts->input_dev);
}
*/

static int hy461x_read_data(struct hy461x_ts_data *ts) {
	struct hy461x_event *event = &ts->event;
	u8 buf[POINT_READ_BUF] = { 0 };
	int ret;
	int i = 0;
	u8 pointid = HY_MAX_ID;

	ret = hy461x_i2c_rxdata(buf, POINT_READ_BUF);
	if (ret < 0) {
		printk("%s: read touch data failed, %d\n", __func__, ret);
		return ret;
	}	
		
	memset(event, 0, sizeof(struct hy461x_event));
#if 0
	event->touch_point = buf[2] & 0x07;
	if (!event->touch_point) 
	{
		hy461x_ts_release(ts);
		return 1;
	}
#endif
	event->touch_point = 0;
	for (i = 0; i < HY461X_PT_MAX; i++) {
		pointid = (buf[HY_TOUCH_ID_POS + HY_TOUCH_STEP * i]) >> 4;
		if (pointid >= HY_MAX_ID)
			break;
        else
            event->touch_point++;

		event-> x[i] =
		    (s16) (buf[HY_TOUCH_X_H_POS + HY_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[HY_TOUCH_X_L_POS + HY_TOUCH_STEP * i];
		event-> y[i] =
		    (s16) (buf[HY_TOUCH_Y_H_POS + HY_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[HY_TOUCH_Y_L_POS + HY_TOUCH_STEP * i];
		event->au8_touch_event[i] =
		    buf[HY_TOUCH_EVENT_POS + HY_TOUCH_STEP * i] >> 6;
		event->au8_finger_id[i] =
		    (buf[HY_TOUCH_ID_POS + HY_TOUCH_STEP * i]) >> 4;
	}
	event->pressure = 200;
	return 0;
}

static void hy461x_ts_pen_irq_work(struct work_struct *work) 
{
	struct hy461x_ts_data *ts = container_of(work, struct hy461x_ts_data, work);
	if (!hy461x_read_data(ts)) 
	{
		hy461x_ts_report(ts);
	}
	enable_irq(this_client->irq);
}

static irqreturn_t hy461x_ts_interrupt(int irq, void *dev_id) 
{
	struct hy461x_ts_data *ts = dev_id;
	disable_irq_nosync(this_client->irq);
	if (!work_pending(&ts->work)) 
	{
		queue_work(ts->queue, &ts->work);
	}
	return IRQ_HANDLED;
}

static int hy461x_power_control(struct hy461x_ts_data *ts ,bool on)
{
    int rc=0;
#if 0
	if(on)
	{
		rc = regulator_enable(ts->vdd);
		if (rc) {
			dev_err(&ts->client->dev,"Regulator vdd enable failed rc=%d\n", rc);
			return rc;
		}

		rc = regulator_enable(ts->vio);
		if (rc) {
			dev_err(&ts->client->dev,"Regulator vcc_io enable failed rc=%d\n", rc);
			regulator_disable(ts->vdd);
		}
    }
    else
	{
		rc = regulator_disable(ts->vdd);
		if (rc) {
			dev_err(&ts->client->dev,"Regulator vdd disable failed rc=%d\n", rc);
			return rc;
		}

		rc = regulator_disable(ts->vio);
		if (rc) {
			dev_err(&ts->client->dev,"Regulator vcc_io disable failed rc=%d\n", rc);
			rc = regulator_enable(ts->vdd);
			if (rc) {
				dev_err(&ts->client->dev,"Regulator vdd enable failed rc=%d\n", rc);
			}
		}
    }
#endif
	return rc;
}

#if defined(CONFIG_FB)
static int hy461x_ts_resume(struct device *dev)
{
    struct hy461x_ts_data *hy461x_data = dev_get_drvdata(dev);

	if (!hy461x_data->suspended) {
		dev_dbg(dev, "Already in awake state\n");
		return 0;
	}

    hy461x_power_control(hy461x_data, true);
	
	if (hy461x_data->pinctrl)
		pinctrl_select_state(hy461x_data->pinctrl,hy461x_data->pins_active);

	if (gpio_is_valid(hy461x_data->pdata->gpio_reset)) {
		gpio_set_value_cansleep(hy461x_data->pdata->gpio_reset, 0);
		msleep(hy461x_data->pdata->hard_rst_dly);
		gpio_set_value_cansleep(hy461x_data->pdata->gpio_reset, 1);
	}

	msleep(hy461x_data->pdata->soft_rst_dly);

	enable_irq(hy461x_data->client->irq);
	hy461x_data->suspended = false;
	
   return 0;
}

static int hy461x_ts_suspend(struct device *dev)
{
    int i;
	struct hy461x_ts_data *hy461x_data = dev_get_drvdata(dev);
	
	if (hy461x_data->suspended) {
		dev_info(dev, "Already in suspend state\n");
		return 0;
	}

	disable_irq(hy461x_data->client->irq);

    hy461x_power_control(hy461x_data, false);

	if (hy461x_data->pinctrl)
		pinctrl_select_state(hy461x_data->pinctrl,hy461x_data->pins_sleep);

	hy461x_data->suspended = true;
	return 0;
}

static int fb_notifier_callback(struct notifier_block *self,
				 unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int *blank;
	struct hy461x_ts_data *hy461x_data =
		container_of(self, struct hy461x_ts_data, fb_notif);

	if (evdata && evdata->data && event == FB_EVENT_BLANK &&
			hy461x_data && hy461x_data->client) {
		blank = evdata->data;
		if (*blank == FB_BLANK_UNBLANK)
			hy461x_ts_resume(&hy461x_data->client->dev);
		else if (*blank == FB_BLANK_POWERDOWN)
			hy461x_ts_suspend(&hy461x_data->client->dev);
	}

	return 0;
}
#elif defined(CONFIG_HAS_EARLYSUSPEND)
static void hy461x_ts_early_suspend(struct early_suspend *handler)
{
	u8 cmd;	   
    msleep(20);
	cmd=PMODE_HIBERNATE;
	printk("hy461x suspend! \n");
	hy461x_write_reg(HY461X_REG_PMODE,&cmd);
}

static void hy461x_ts_early_resume(struct early_suspend *handler)
{
	printk("hy461x resume!\n");	
	gpio_direction_output(gpio_reset, 0);
	msleep(10);
	gpio_direction_output(gpio_reset, 1);
	msleep(10);
}
#endif




#ifdef HY_UPDATE_FW_ONLINE

static void hy461x_ts_reset(void)
{

	gpio_direction_output(gpio_reset, 0);
	msleep(10);
	gpio_direction_output(gpio_reset, 1);
	msleep(50);
}

void hy461x_read_BootloaderVer(u8 *Boot_Ver)
{
	u8 read_cmd[2]= {0xff,0x90};
	u8 cmd_len 	= 0;
	u8 data_len = 0;
	*Boot_Ver = 0xff;
	
	cmd_len  = 2;
	data_len = 2;	

	/*send register addr*/
	if(!i2c_write_interface(&read_cmd[0], cmd_len))
	{
		printk("%s %d   i2c_write_interface\n",__FUNCTION__,__LINE__);
	}
	msleep(5);
	/*call the read callback function to get the register value*/		
	if(!i2c_read_interface(Boot_Ver, data_len))
	{
		printk("%s %d  i2c_read_interface\n",__FUNCTION__,__LINE__);
	}

}

static unsigned char hy461x_read_app_dataflash_checksum(void)
{
	u8 write_cmd[6]= {0xff,0x62,0x10,0x00,0xef,0x00};//app+dataflash checksum
	u8 cmd_len 	= 6;
	u8 data_len = 1;	
	u8 checksum = 0;

	/*send register addr*/
	if(!i2c_write_interface(&write_cmd[0], cmd_len))
	{
		return 0;
	}	
	msleep(200);	
	/*call the read callback function to get the register value*/		
	if(!i2c_read_interface(&checksum, data_len))
	{
		return 0;
	}
	printk("TSP FW_CheckSum = %d \n",checksum);	
	return checksum;
}

static void hy461x_saveBasedata(void)
{
		u8 write_cmd[6]= {0xb1,0x3c,0x03,0x3d,0x00,0x00};
		u8 cmd_len 	= 6;
		//u8 data_len = 1;
		char Base_val = 0xff;
		int ret;
		int i = 0;
	
		do{
		if(!i2c_write_interface(&write_cmd[0], cmd_len))
		{
			printk("%s %d   i2c_write_interface\n",__FUNCTION__,__LINE__);
		}	
		msleep(500);
		
		ret = hy461x_read_reg(HY461X_REG_BASE, &Base_val, 1);
		if(ret<0)
				printk("Read 0XAE faild!!\n");
		i++;
		}while((i<5)&&(0x55 != Base_val));
		if(Base_val != 0x55)
				printk("Save Basedata faild!!\n");
}

static void Flash_write(void)
{
	u8 write_cmd[6]= {0xff,0x63,0x03,0x3d,0x00,0x00};
	u8 cmd_len 	= 6;
	u8 data_len = 1;
	char W_flash_open = 0xff;
	
	do{
		/*send register addr*/
		if(!i2c_write_interface(&write_cmd[0], cmd_len))
		{
			printk("%s %d   i2c_write_interface\n",__FUNCTION__,__LINE__);
		}	
		msleep(5);
		/*call the read callback function to get the register value*/		
		if(!i2c_read_interface(&W_flash_open, data_len))
		{
			printk("%s %d   i2c_read_interface\n",__FUNCTION__,__LINE__);
		}
	}while(0x3d != W_flash_open);
	printk("TSP W_flash_open = 0x%02x \n",W_flash_open);	
}

static bool cmd_write(u8 btcmd,u8 btPara1,u8 btPara2,u8 btPara3,u8 num)
{
    u8 write_cmd[4] = {0};

    write_cmd[0] = btcmd;
    write_cmd[1] = btPara1;
    write_cmd[2] = btPara2;
    write_cmd[3] = btPara3;
    return i2c_write_interface(write_cmd, num);
}
static bool byte_write(u8* buffer, int length)
{
    
    return i2c_write_interface(buffer, length);
}
#define    HYS_PACKET_LENGTH        128
static unsigned char CTPM_FW[]=
{
#include "firmware.i"
};

static unsigned char hys_i_file_checksum(void)
{
    unsigned int CTPM_FW_len,i;
    unsigned char i_checksum ;	
    CTPM_FW_len = sizeof(CTPM_FW);
	
    i_checksum = 0;
    for(i=0; i<CTPM_FW_len; i++)
	{
		i_checksum += CTPM_FW[i];
	}
	
	printk("I File Checksum = %d \n",i_checksum);
    return i_checksum;        
}

E_UPGRADE_ERR_TYPE  hys_ctpm_fw_upgrade(u8* pbt_buf, int dw_lenth)
{
    u8  packet_buf[HYS_PACKET_LENGTH + 5];
    u8  bt_ecc = 0;
    int  j,temp,lenght,packet_number, i;
    u16 Boot_Ver = 0;

  printk("=======[HYCON] STEP 1 :Reset CTP. \n"); 
	hy461x_ts_reset();
	printk("=======[HYCON] STEP 2 :Read Bootloader Version. \n");
	hy461x_read_BootloaderVer((u8*)&Boot_Ver);
	Boot_Ver =  ((Boot_Ver&0x00ff)<<8)  + ((Boot_Ver&0xff00)>>8);
	printk("[TSP] IC BOOT version = 0x%02x\n", Boot_Ver);
	Boot_Ver = ((Boot_Ver&0xff00)>>8);	
	if(0x46 == Boot_Ver)
	{
		printk("=======[HYCON] STEP 3 :Erase Flash. \n");
		cmd_write(0xff,0x91,0x00,0x00,2);//erase app area
		msleep(500);//Delay 500ms
		printk("=======[HYCON] STEP 4 :write firmware(FW) to flash. \n");
		packet_number = (dw_lenth) / HYS_PACKET_LENGTH;
		packet_buf[0] = 0xbf;
		lenght = HYS_PACKET_LENGTH;
		packet_buf[3] = (u8)(lenght>>8);
		packet_buf[4] = (u8)lenght;	
		for (j=0;j<packet_number;j++)
		{
			temp = j * HYS_PACKET_LENGTH;
			packet_buf[1] = (u8)(temp>>8);
			packet_buf[2] = (u8)temp;		
			
			for (i = 0;i < HYS_PACKET_LENGTH; i++)
			{			
				packet_buf[5+i] = pbt_buf[j*HYS_PACKET_LENGTH + i]; 
				#ifdef HY_CHECK_BUF_UP
				if(0xff != pbt_buf[j*HYS_PACKET_LENGTH + i])
				{
					bt_ecc = 1;	
				}	
				#else
					bt_ecc = 1;	
				#endif /*end #ifdef HY_CHECK_BUF_UP */	
			}
			if(1 == bt_ecc)
			{
				byte_write(&packet_buf[0], HYS_PACKET_LENGTH + 5);
				bt_ecc = 0;
				msleep(25);
				printk("HYCON FW Update Buffer %d\n",j);	
			}		
		}
	}
	if(0x10 == Boot_Ver)
	{
		printk("=======[HYCON] STEP 3 :Flash Write Open. \n");
		Flash_write();
		printk("=======[HYCON] STEP 4 :Erase Flash. \n");
		cmd_write(0xff,0x91,0x00,0x00,2);//erase app area
		msleep(1500);//Delay 1500ms
		printk("=======[HYCON] STEP 5 :write firmware(FW) to flash. \n");
		packet_number = (dw_lenth) / HYS_PACKET_LENGTH;
		packet_buf[0] = 0xbf;
		lenght = HYS_PACKET_LENGTH;
		packet_buf[3] = (u8)(lenght>>8);
		packet_buf[4] = (u8)lenght;	
		for (j=0;j<packet_number;j++)
		{
			temp = j * HYS_PACKET_LENGTH;
			packet_buf[1] = (u8)(temp>>8);
			packet_buf[2] = (u8)temp;		
			
			for (i = 0;i < HYS_PACKET_LENGTH; i++)
			{			
				packet_buf[5+i] = pbt_buf[j*HYS_PACKET_LENGTH + i]; 
				#ifdef HY_CHECK_BUF_UP	
				if(0xff != pbt_buf[j*HYS_PACKET_LENGTH + i])
				{
					bt_ecc = 1;	
				}
				#else
					bt_ecc = 1;
				#endif /*end #ifdef HY_CHECK_BUF_UP */		
			}
			if(1 == bt_ecc)
			{
				byte_write(&packet_buf[0], HYS_PACKET_LENGTH + 5);
				bt_ecc = 0;
				msleep(25);
				printk("HYCON FW Update Buffer %d\n",j);	
			}		
		}

		//Read ECC
	   printk("=======[HYCON] STEP 6 :Read ECC. \n");
	   if(hys_i_file_checksum() != hy461x_read_app_dataflash_checksum())
		{
			printk("FW Update Fail, Please try again!");
		}
	}	
   //reset the CTP after finish update fw
   printk("=======[HYCON] Last STEP :Reset the CTP. \n");
   hy461x_ts_reset();
   msleep(450);
   //save base data
   //msleep(1000);
   //hy461x_saveBasedata();
   //return  ERR_OK;
}
int hys_ctpm_fw_upgrade_with_i_file(void)
{
   u8* pbt_buf = NULL;
   int i_ret;   
   pbt_buf = CTPM_FW;
   //i_ret =  hys_ctpm_fw_upgrade(pbt_buf,(sizeof(CTPM_FW)-1024));
  i_ret =  hys_ctpm_fw_upgrade(pbt_buf,(sizeof(CTPM_FW)));  
   return i_ret;
}
unsigned char hys_ctpm_get_upg_ver(void)
{
    unsigned int ui_sz;	
    ui_sz = sizeof(CTPM_FW);
    if (ui_sz > 2)
    {
       // return CTPM_FW[ui_sz - 2];
	return CTPM_FW[ui_sz - 1];
    }
    else
        return 0xff; 
}
#endif //end HY_UPDATE_FW_ONLINE

#define OF_TP_SUPPLY_VDD    "vdd"
#define OF_TP_SUPPLY_VIO  "vcc_i2c"
#define OF_TP_PIN_RESET "qcom,ts-gpio-reset"
#define OF_TP_PIN_IRQ   "qcom,ts-gpio-irq"
#define OF_TP_MAX_X     "qcom,max-x"
#define OF_TP_MAX_Y     "qcom,max-y"

#define TP_PINCTR_RELESE  "pmx_ts_release"
#define TP_PINCTR_ACTIVE  "pmx_ts_active"
#define TP_PINCTR_SUSPEND "pmx_ts_suspend"

static int hy461x_parse_dt(struct device *dev,
			struct hy461x_i2c_platform_data *pdata)
{
	struct device_node *np = dev->of_node;
    int ret;
	gpio_reset = pdata->gpio_reset =of_get_named_gpio_flags(dev->of_node, OF_TP_PIN_RESET, 0, NULL);
	if (pdata->gpio_reset <= 0) {
		dev_err(dev,"error getting gpio for %s\n",  OF_TP_PIN_RESET);
		ret = -EINVAL;
	}

	pdata->gpio_irq =of_get_named_gpio_flags(dev->of_node,OF_TP_PIN_IRQ, 0, NULL);
	if (pdata->gpio_irq <= 0) {
		dev_err(dev,"error getting gpio for %s\n",  OF_TP_PIN_IRQ);
		ret = -EINVAL;
	}

	ret = of_property_read_u32(np, OF_TP_MAX_X, &pdata->screen_max_x);
	if (ret) {
		dev_err(dev, "error getting screen_max_x\n");
		ret = -EINVAL;
	}
	ret = of_property_read_u32(np, OF_TP_MAX_Y, &pdata->screen_max_y);
	if (ret) {
		dev_err(dev, "error getting screen_max_y\n");
		ret = -EINVAL;
	}

	ret = of_property_read_u32(np, "qcom,hard-reset-delay-ms",&pdata->hard_rst_dly);
	if (ret) {
		dev_err(dev, "qcom,hard-reset-delay-ms");
		ret = -EINVAL;
	}

	ret = of_property_read_u32(np, "qcom,soft-reset-delay-ms",&pdata->soft_rst_dly);
	if (ret) {
		dev_err(dev, "qcom,soft-reset-delay-ms\n");
		ret = -EINVAL;
	}
	return ret;
}

static int hy461x_pinctrl_init(struct hy461x_ts_data *ts)
{

	ts->pinctrl = devm_pinctrl_get(&ts->client->dev);
	if (IS_ERR(ts->pinctrl)) {
		dev_err(&ts->client->dev, "no pinctrl\n");
		return 0;
	}

	ts->pins_default = pinctrl_lookup_state(ts->pinctrl, TP_PINCTR_RELESE);
	if (IS_ERR(ts->pins_default))
		dev_err(&ts->client->dev, "could not get default pinstate\n");

	ts->pins_sleep = pinctrl_lookup_state(ts->pinctrl, TP_PINCTR_SUSPEND);
	if (IS_ERR(ts->pins_sleep))
		dev_err(&ts->client->dev, "could not get sleep pinstate\n");

	ts->pins_active = pinctrl_lookup_state(ts->pinctrl, TP_PINCTR_ACTIVE);
	if (IS_ERR(ts->pins_active))
		dev_err(&ts->client->dev, "could not get inactive pinstate\n");
	return 0;
	
}

static int hy461x_power_init(struct hy461x_ts_data *ts)
{
	struct regulator *ldo;

	ldo = regulator_get(&ts->client->dev, OF_TP_SUPPLY_VDD);
	ts->vdd = ldo;
	if (IS_ERR(ldo)) {
		dev_warn(&ts->client->dev, "can't get %s-supply handle\n", OF_TP_SUPPLY_VDD);
		ts->vdd = NULL;
	}

	ldo = regulator_get(&ts->client->dev, OF_TP_SUPPLY_VIO);
	ts->vio = ldo;
	if (IS_ERR(ldo)) {
		dev_warn(&ts->client->dev, "can't get %s-supply handle\n", OF_TP_SUPPLY_VIO);
		ts->vio = NULL;
	}
    return 0;
}
	
static int hy461x_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct hy461x_i2c_platform_data *pdata;
	struct hy461x_ts_data *ts;
	struct input_dev *input_dev;
	unsigned char val;
	int err = -EINVAL;

	if (client->dev.of_node) {
		pdata = devm_kzalloc(&client->dev,sizeof(struct hy461x_i2c_platform_data), GFP_KERNEL);
		if (!pdata) {
			dev_err(&client->dev, "Failed to allocate memory\n");
			return -ENOMEM;
		}

		err = hy461x_parse_dt(&client->dev, pdata);
		if (err) {
			dev_err(&client->dev, "DT parsing failed\n");
			return err;
		}
	} else
		pdata = client->dev.platform_data;

	if (!pdata) {
		dev_err(&client->dev, "Invalid pdata\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}

	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (!ts) {
		err = -ENOMEM;
		goto exit_alloc_data_failed;
	}
	
    /*
	pdata = client->dev.platform_data;
	if (!pdata) {
		dev_err(&client->dev, "failed to get platform data\n");
		goto exit_no_pdata;
	}
	
	ts->screen_max_x = pdata->screen_max_x;
	ts->screen_max_y = pdata->screen_max_y;
	ts->pressure_max = pdata->pressure_max;
    */
	
	ts->gpio_irq = pdata->gpio_irq;
	if (ts->gpio_irq != -EINVAL) {
		client->irq = gpio_to_irq(ts->gpio_irq);
	}
    
    /*
	if (pdata->irq_cfg) {
		s3c_gpio_cfgpin(ts->gpio_irq, pdata->irq_cfg);
		s3c_gpio_setpull(ts->gpio_irq, S3C_GPIO_PULL_NONE);
	}

	ts->gpio_wakeup = pdata->gpio_wakeup;
    */
	ts->gpio_reset = pdata->gpio_reset;

	INIT_WORK(&ts->work, hy461x_ts_pen_irq_work);
	this_client = client;
	i2c_set_clientdata(client, ts);

	ts->queue = create_singlethread_workqueue(dev_name(&client->dev));
	if (!ts->queue) {
		err = -ESRCH;
		goto exit_create_singlethread;
	}

	//hy461x_power_init(ts);
	//hy461x_power_control(ts,true);
	//hy461x_pinctrl_init(ts);


	input_dev = input_allocate_device();
	if (!input_dev) {
		err = -ENOMEM;
		dev_err(&client->dev, "failed to allocate input device\n");
		goto exit_input_dev_alloc_failed;
	}

	ts->input_dev = input_dev;
	ts->client = client;
	ts->pdata = pdata;
	
	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);
    set_bit(BTN_TOUCH, input_dev->keybit);

printk("********************************************\n");
printk("Report coordinates: X = %d , Y = %d \n",ts->pdata->screen_max_x,ts->pdata->screen_max_y);
printk("********************************************\n");
	input_set_abs_params(input_dev, ABS_X, 0, ts->pdata->screen_max_x, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, ts->pdata->screen_max_y, 0, 0);
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 200, 0, 0);

	input_dev->name = HY461X_NAME;
	input_dev->id.bustype = BUS_I2C;
	input_dev->id.vendor = 0x12FA;
	input_dev->id.product = 0x2143;
	input_dev->id.version = 0x0100;

	err = input_register_device(input_dev);
	if (err) {
		input_free_device(input_dev);
		dev_err(&client->dev, "failed to register input device %s, %d\n",
				dev_name(&client->dev), err);
		goto exit_input_dev_alloc_failed;
	}

	hy461x_power_init(ts);
	hy461x_power_control(ts, true);
	hy461x_pinctrl_init(ts);

	#ifdef HY_UPDATE_FW_ONLINE
    //read IC FW version
	hy461x_ts_reset();
	msleep(50);//waiting for read firmware version
	printk("Read IC Firmware version and I file version:\n");
	hy461x_read_fw_ver(&val);	
	printk("[TSP] IC firmware version = 0x%02x\n", val);
	printk("[TSP] I file verison = 0x%02x\n", hys_ctpm_get_upg_ver());
	if (hys_ctpm_get_upg_ver() > val)  
    {
        printk("[TSP] start upgrade new verison 0x%02x\n", hys_ctpm_get_upg_ver());
        msleep(5);
       // hys_ctpm_fw_upgrade_with_i_file();
        err =  hys_ctpm_fw_upgrade_with_i_file();
        if (err == 0)
		{
			msleep(5);
			printk("[TSP] ugrade successfuly ver=%x.\n",val_upgrade);
			hy461x_read_fw_ver(&reg_value);
			printk("HYS_DBG from old version 0x%02x to new version = 0x%02x\n", val, reg_value);
		}
		else
		{
			printk("[TSP]  ugrade fail err=%d, line = %d.\n",
                err, __LINE__);
		}
		/*
		if (err == 0)
        {
            printk("[TSP] ugrade successfuly.\n");
            msleep(5);
            hy461x_read_fw_ver(&reg_value);
            printk("HYS_DBG from old version 0x%02x to new version = 0x%02x\n", val, reg_value);
        }
        else
        {
            printk("[TSP]  ugrade fail err=%d, line = %d.\n",
                err, __LINE__);
        }   
		*/		
        msleep(4000);
    }
#endif

	if (gpio_is_valid(ts->pdata->gpio_reset)) {
		gpio_set_value_cansleep(ts->pdata->gpio_reset, 0);
		msleep(ts->pdata->hard_rst_dly);
		gpio_set_value_cansleep(ts->pdata->gpio_reset, 1);
	}

	msleep(ts->pdata->soft_rst_dly);	

	err = hy461x_read_fw_ver(&val);
	if (err < 0) {
		dev_err(&client->dev, "chip not found\n");
		goto exit_irq_request_failed;
	}

	err = request_irq(client->irq, hy461x_ts_interrupt,
			IRQ_TYPE_EDGE_FALLING /*IRQF_TRIGGER_FALLING*/, "hy461x_ts", ts);
	if (err < 0) {
		dev_err(&client->dev, "Request IRQ %d failed, %d\n", client->irq, err);
		goto exit_irq_request_failed;
	}

	disable_irq(client->irq);

	dev_info(&client->dev, "Firmware version 0x%02x\n", val);

#if defined(CONFIG_FB)
	ts->fb_notif.notifier_call = fb_notifier_callback;

	err = fb_register_client(&ts->fb_notif);

	if (err)
		dev_err(&client->dev, "Unable to register fb_notifier: %d\n",err);
	
#elif defined(CONFIG_HAS_EARLYSUSPEND)
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ts->early_suspend.suspend = hy461x_ts_early_suspend;
	ts->early_suspend.resume = hy461x_ts_early_resume;
	register_early_suspend(&ts->early_suspend);
#endif

	enable_irq(client->irq);

	dev_info(&client->dev, "Hycon hy461x TouchScreen initialized\n");
	return 0;

exit_irq_request_failed:
	hy461x_power_control(ts, false);
	input_unregister_device(input_dev);

exit_input_dev_alloc_failed:
	cancel_work_sync(&ts->work);
	destroy_workqueue(ts->queue);

exit_create_singlethread:
	i2c_set_clientdata(client, NULL);

//exit_no_pdata:
//	kfree(ts);

exit_alloc_data_failed:
exit_check_functionality_failed:
	dev_err(&client->dev, "probe hy461x TouchScreen failed, %d\n", err);

	return err;
	
}

static int hy461x_ts_remove(struct i2c_client *client) 
{
	struct hy461x_ts_data *ts = i2c_get_clientdata(client);

#if defined(CONFIG_FB)
	if (fb_unregister_client(&ts->fb_notif))
		dev_err(&ts->client->dev, "Error occurred while unregistering fb_notifier.\n");
#elif defined (CONFIG_HAS_EARLYSUSPEND)
	unregister_early_suspend(&ts->early_suspend);
#endif

	if (client->irq) {
		free_irq(client->irq, ts);
	}

	cancel_work_sync(&ts->work);
	destroy_workqueue(ts->queue);

	i2c_set_clientdata(client, NULL);
	hy461x_power_control(ts, false);
	
	input_unregister_device(ts->input_dev);
	if (ts->input_dev)
		kfree(ts->input_dev);

	kfree(ts);

	return 0;
}

static struct of_device_id hy461x_match_table[] = {
	{ .compatible = "qcom,hy461x",},
	{ },
};

static const struct i2c_device_id hy461x_ts_id[] = {
	{ HY461X_NAME, 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, hy461x_ts_id);

static struct i2c_driver hy461x_ts_driver = {
	.probe		= hy461x_ts_probe,
	.remove		= hy461x_ts_remove,
	.id_table	= hy461x_ts_id,
	.driver	= {
		.name	= HY461X_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = hy461x_match_table,
	},
};

static int __init hy461x_ts_init(void)
{
	return i2c_add_driver(&hy461x_ts_driver);
}

static void __exit hy461x_ts_exit(void)
{
	i2c_del_driver(&hy461x_ts_driver);
}

module_init(hy461x_ts_init);
module_exit(hy461x_ts_exit);

MODULE_AUTHOR("<TP>");
MODULE_DESCRIPTION("FocalTech hy461x TouchScreen driver");
MODULE_LICENSE("GPL");

