#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "aht20reg.h"

#define AHT20_CNT	1
#define AHT20_NAME	"aht20"

/* aht20设备结构体 */
struct aht20_dev {
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;	/* 类 		*/
	struct device *device;	/* 设备 	 */
	struct device_node	*nd; /* 设备节点 */
	int major;			/* 主设备号 */
	void *private_data;	/* 私有数据 */
	int temperature, humidity;		/* 温度和湿度 */
};

static struct aht20_dev aht20dev;


/**************************************************************************************/
/*
 * @description	: 从aht20读取多个寄存器数据
 * @param - dev:  aht20设备
 * @param - buf:  读取到的数据
 * @param - len:  要读取的数据长度
 * @return 		: 操作结果
 */
static int aht20_read_regs(struct aht20_dev *dev, u8 *buf, int len)
{
    int ret;
    struct i2c_client *client = (struct i2c_client *)dev->private_data; //i2c_transfer函数的第一个参数
    struct i2c_msg msg;              //i2c_transfer函数的第二个参数
    msg.addr  = client->addr;
    msg.flags = I2C_M_RD;
    msg.buf   = buf;
    msg.len   = len;
    
    ret = i2c_transfer(client->adapter, &msg, 1);    //函数第一个参数是i2c_adpater,i2c_client会保存对应的i2c_adpater

    if(ret == 1) {
		ret = 0;
	} else {
		printk("i2c rd failed=%d reg=%06x len=%d\n", ret, client->addr, len);
		ret = -EREMOTEIO;
	}
	return ret;
}


/*
 * @description	: 向aht20多个寄存器写入数据
 * @param - dev:  aht20设备
 * @param - buf:  要写入的数据缓冲区
 * @param - len:  要写入的数据长度
 * @return 	  :   操作结果
 */
static s32 aht20_write_regs(struct aht20_dev *dev, u8 *buf, u8 len)
{
	struct i2c_msg msg;
	struct i2c_client *client = (struct i2c_client *)dev->private_data;

    msg.addr = client->addr;	/* aht20地址 */
	msg.flags = 0;				/* 标记为写数据 */
	msg.buf = buf;				/* 要写入的数据缓冲区 */
	msg.len = len;			    /* 要写入的数据长度 */

    return i2c_transfer(client->adapter, &msg, 1);
}


/*
 * @description	: 读取aht20指定寄存器值，读取一个寄存器
 * @param - dev:  aht20设备
 * @return 	  :   读取到的寄存器值
 */
static unsigned char aht20_read_reg(struct aht20_dev *dev)
{
    u8 data = 0;

    aht20_read_regs(&aht20dev, &data, 1);

    return data;
}

/*
 * @description	: 向aht20指定寄存器写入指定的值，写一个寄存器
 * @param - dev:  aht20设备
 * @param - data: 要写入的值
 * @return   :    无
 */
static void aht20_write_reg(struct aht20_dev *dev, u8 data)
{
    u8 buf = 0;
    buf = data;
    aht20_write_regs(dev, &data, 1);
}

/*
 * @brief       读取AHT20的状态寄存器
 * @param       无
 * @retval      状态值
 */
uint8_t atk_aht20_read_status(void)
{
    uint8_t ret;
    
    ret = aht20_read_reg(&aht20dev);                 /* 读取状态寄存器 */

    //如果读取成功，ret = 0x08
    return ret;
}

/*
 * @brief       检测AHT20传感器是否就绪
 * @param       无
 * @retval      0, 正常
 *              1, 异常/不存在
 */
uint8_t atk_aht20_check(void)
{
    if((atk_aht20_read_status() & 0x08) == 0x08)    /* 校准输出被启用？ */
    {
        return 0;       /* 正常，返回0 */
    }
    else
    {
        return 1;       /* 异常，返回1 */
    }
}

/*
 * @description	: 读取aht20的数据，读取原始数据，包括温度和湿度, 注意！
 *				: 
 * @param - ir	: ir数据
 * @param - ps 	: ps数据
 * @param - ps 	: als数据 
 * @return 		: 无。
 */

void aht20_readdata(struct aht20_dev *dev)
{
    uint8_t raw_data[7];                                   //读取的原始值
    uint32_t humi_data = 0, temp_data = 0;                  //湿度和温度变量

    uint8_t command[3] = {0xAC, 0x33, 0x00};

    aht20_write_regs(&aht20dev, command, 3);                /* 发送测量指令 */
    mdelay(80);                                             /* 等待80ms */

    aht20_read_regs(&aht20dev, raw_data, 7);                /* 获取原始数据 */

    if((raw_data[0] & 0x80) == 0x00)
    {
        // // 湿度：raw[1]全部, raw[2]全部, raw[3]的高4位
        // humi_data = ((uint32_t)raw_data[1] << 12) | ((uint32_t)raw_data[2] << 4) | (raw_data[3] >> 4);
        // // 温度：raw[3]低4位, raw[4]全部, raw[5]全部
        // temp_data = ((uint32_t)(raw_data[3] & 0x0F) << 16) | ((uint32_t)raw_data[4] << 8) | raw_data[5];
        // dev->humidity = humi_data;
        // dev->temperature = temp_data;
        humi_data = 0;                                      /* 变量清零 */
        humi_data = (humi_data | raw_data[1]<< 12);         /* 取出第一个字节湿度数据，即[19:12]位 */
        humi_data = (humi_data | raw_data[2]<< 4);          /* 取出第二个字节湿度数据，即[11:4]位 */
        humi_data = (humi_data | raw_data[3] >> 4);         /* 取出第三个字节湿度数据，即[3:0]位 */
        /* 湿度有效数据共有20位，第三个字节数据只有高4位有效，因此需要右移4位，才能得到有效数据 */
        dev->humidity = humi_data;
        //dev->humidity = (float)humi_data * 100 /1024 /1024;     /* 计算湿度值 */

        temp_data = 0;                                      /* 变量清零 */
        temp_data = (temp_data | raw_data[3]<< 16);         /* 取出第一个字节温度数据，即[19:16]位 */
        temp_data = (temp_data | raw_data[4]<< 8) ;         /* 取出第二个字节温度数据，即[15:8]位 */
        temp_data = (temp_data | raw_data[5]);              /* 取出第三个字节温度数据，即[7:0]位 */
        temp_data = temp_data & 0xfffff;                    /* 温度有效数据共有20位，第一个字节数据只有低4位有效，因此需要右移4位，才能得到有效数据 */
        dev->temperature = temp_data;
        //dev->temperature = (float)temp_data * 200 / 1024 / 1024-50;/* 计算温度值 */
    }
}
/**************************************************************************************/

/*******************************aht20操作函数********************************************/
static int aht20_open(struct inode *inode, struct file *filp)
{
    int ret = 0;
    uint8_t command[3] = {INIT, 0x08, 0x00};
    filp->private_data = &aht20dev;

    /* 初始化aht20 */
    mdelay(40);                                         /* 延时40ms，等硬件稳定 */
    ret = aht20_write_regs(&aht20dev, command, 3);            /* 发送初始化指令 */
    if (ret < 0) {
        printk("aht20: init failed\n");
        return -EIO;
    }
    mdelay(500);                                        /* 延时500ms，等硬件稳定 */

    if(atk_aht20_check()!=0) {                          /* 检测AHT20的状态,成功返回0 */
        printk("aht20 check failed!\n");
        return -ENODEV;
    }

    printk("aht20 check success!\n");
    return 0;
}

static ssize_t aht20_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    int data[2];
	long err = 0;

    struct aht20_dev *dev = (struct aht20_dev *)filp->private_data;

    aht20_readdata(dev);      //读取传感器数据
    data[0] = dev->temperature;
    data[1] = dev->humidity;

    err = copy_to_user(buf, data, sizeof(data));

    return sizeof(data);
}

static int aht20_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* aht20操作函数 */
static const struct file_operations aht20_ops = {
	.owner = THIS_MODULE,
	.open = aht20_open,
	.read = aht20_read,
	.release = aht20_release,
};
/*****************************************************************************************/

 /*
  * @description     : i2c驱动的probe函数，当驱动与
  *                    设备匹配以后此函数就会执行
  * @param - client  : i2c设备
  * @param - id      : i2c设备ID
  * @return          : 0，成功;其他负值,失败
  */
static int aht20_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    /* 1、构建设备号 */
    if(aht20dev.major) {      //如果给定主设备号
        aht20dev.devid = MKDEV(aht20dev.major, 0);
        register_chrdev_region(aht20dev.devid, AHT20_CNT, AHT20_NAME);
    } else {
		alloc_chrdev_region(&aht20dev.devid, 0, AHT20_CNT, AHT20_NAME);
		aht20dev.major = MAJOR(aht20dev.devid);
	}

    /* 2、注册设备 */
    cdev_init(&aht20dev.cdev, &aht20_ops);
    cdev_add(&aht20dev.cdev, aht20dev.devid, AHT20_CNT);

    /* 3、创建类 */
    aht20dev.class = class_create(THIS_MODULE, AHT20_NAME);
    if (IS_ERR(aht20dev.class)) {
		return PTR_ERR(aht20dev.class);
	}

    /* 4、创建设备 */
    aht20dev.device = device_create(aht20dev.class, NULL, aht20dev.devid, NULL, AHT20_NAME);
	if (IS_ERR(aht20dev.device)) {
		return PTR_ERR(aht20dev.device);
	}

    aht20dev.private_data = client;//把client存到全局变量aht20dev中，在aht20读写寄存器函数中可以取出client

    return 0;
}

/*
 * @description     : i2c驱动的remove函数，移除i2c驱动的时候此函数会执行
 * @param - client 	: i2c设备
 * @return          : 0，成功;其他负值,失败
 */
static int aht20_remove(struct i2c_client *client)
{
    /* 删除设备 */
    cdev_del(&aht20dev.cdev);
	unregister_chrdev_region(aht20dev.devid, AHT20_CNT);

    /* 注销掉类和设备 */
    device_destroy(aht20dev.class, aht20dev.devid);
	class_destroy(aht20dev.class);

    return 0;
}

/* 传统匹配方式ID列表 */
static struct i2c_device_id aht20_id[] = {
	{"alientek,aht20", 0},  
	{ /* Sentinel */ }
};

/* 设备树匹配表 */
struct of_device_id aht20_of_match[] = {
    {.compatible = "alientek,aht20"},         //与设备树中i2c1节点下的aht20器件的compatible要对应
    { /* Sentinel */ }
};

/* i2c_driver结构体 */
static struct i2c_driver aht20_driver = {
    .probe = aht20_probe,
    .remove = aht20_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "aht20",                      //决定了驱动在 /sys/bus/i2c/drivers/ 目录下显示的文件夹名字
        .of_match_table = aht20_of_match,      //用于和设备树中的compatible属性匹配
    },
    .id_table = aht20_id,                     //传统匹配方式,无设备树时与i2c_board_info进行匹配
};


/* 驱动入口函数 */
static int __init aht20_init(void)
{
    int ret = 0;

    ret = i2c_add_driver(&aht20_driver);

    return ret;
}

/* 驱动出口函数 */
static void __exit aht20_exit(void)
{
    i2c_del_driver(&aht20_driver);
}

module_init(aht20_init);
module_exit(aht20_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWY");
