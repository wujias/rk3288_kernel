#ifndef _RELAYIO_H_
#define _RELAYIO_H_

/* 定义幻数 */
#define MEMDEV_IOC_MAGIC  'R'

#define RELAY_ON                _IO(MEMDEV_IOC_MAGIC,1)
#define RELAY_OFF               _IO(MEMDEV_IOC_MAGIC,2)
#define SET_IO_TRIG_HIGH        _IO(MEMDEV_IOC_MAGIC,3)
#define SET_IO_TRIG_LOW         _IO(MEMDEV_IOC_MAGIC,4)
/*
#define LED0_ON                 _IO(MEMDEV_IOC_MAGIC,5)
#define LED0_OFF                _IO(MEMDEV_IOC_MAGIC,6)
#define LED1_ON                 _IO(MEMDEV_IOC_MAGIC,7)
#define LED1_OFF                _IO(MEMDEV_IOC_MAGIC,8)
*/
#endif /* _MEMDEV_H_ */