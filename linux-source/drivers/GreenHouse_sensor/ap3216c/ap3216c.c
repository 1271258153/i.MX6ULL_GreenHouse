#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "ap3216creg.h"
#include <linux/regmap.h>

#define AP3216C_CNT	1
#define AP3216C_NAME	"ap3216c"

/* ap3216c设备结构体 */
struct ap3216c_dev {
	dev_t devid;			/* 设备号 	 */
	struct cdev cdev;		/* cdev 	*/
	struct class *class;	/* 类 		*/
	struct device *device;	/* 设备 	 */
	struct device_node	*nd; /* 设备节点 */
	int major;			/* 主设备号 */
	void *private_data;	/* 私有数据 */
	unsigned short ir, als, ps;		/* 三个光传感器数据 */
    struct regmap *regmap;				/* regmap */
	struct regmap_config regmap_config;	
};

static struct ap3216c_dev ap3216cdev;


/**************************************************************************************/
/*
//  * @description	: 从ap3216c读取多个寄存器数据
//  * @param - dev:  ap3216c设备
//  * @param - reg:  要读取的寄存器首地址
//  * @param - val:  读取到的数据
//  * @param - len:  要读取的数据长度
//  * @return 		: 操作结果
//  */
// static int ap3216c_read_regs(struct ap3216c_dev *dev, u8 reg, void *val, int len)
// {
//     int ret;
//     struct i2c_client *client = (struct i2c_client *)dev->private_data; //i2c_transfer函数的第一个参数
//     struct i2c_msg msg[2];              //i2c_transfer函数的第二个参数

//     /* msg[0]为发送要读取的首地址，参考i2c读时序 */
//     msg[0].addr = client->addr;         /* ap3216c地址 */
//     msg[0].flags = 0;					/* 标记为发送数据 */
//     msg[0].buf = &reg;                  /* 读取的首地址 */
//     msg[0].len = 1;						/* reg长度*/

//     /* msg[1]读取数据 */
//     msg[1].addr = client->addr;			/* ap3216c地址 */
// 	msg[1].flags = I2C_M_RD;			/* 标记为读取数据*/
// 	msg[1].buf = val;					/* 读取数据缓冲区 */
// 	msg[1].len = len;					/* 要读取的数据长度*/
    
//     ret = i2c_transfer(client->adapter, msg, 2);    //函数第一个参数是i2c_adpater,i2c_client会保存对应的i2c_adpater

//     if(ret == 2) {
// 		ret = 0;
// 	} else {
// 		printk("i2c rd failed=%d reg=%06x len=%d\n",ret, reg, len);
// 		ret = -EREMOTEIO;
// 	}
// 	return ret;
// }


// /*
//  * @description	: 向ap3216c多个寄存器写入数据
//  * @param - dev:  ap3216c设备
//  * @param - reg:  要写入的寄存器首地址
//  * @param - val:  要写入的数据缓冲区
//  * @param - len:  要写入的数据长度
//  * @return 	  :   操作结果
//  */
// static s32 ap3216c_write_regs(struct ap3216c_dev *dev, u8 reg, u8 *buf, u8 len)
// {
//     u8 b[256];
// 	struct i2c_msg msg;
// 	struct i2c_client *client = (struct i2c_client *)dev->private_data;

//     b[0] = reg;					/* b[0]保存寄存器首地址 */
//     memcpy(&b[1], buf, len);    /* 将要写入的数据拷贝到数组b里面 */

//     msg.addr = client->addr;	/* ap3216c地址 */
// 	msg.flags = 0;				/* 标记为写数据 */

// 	msg.buf = b;				/* 要写入的数据缓冲区 */
// 	msg.len = len + 1;			/* 要写入的数据长度 */

//     return i2c_transfer(client->adapter, &msg, 1);
// }


/*
 * @description	: 读取ap3216c指定寄存器值，读取一个寄存器
 * @param - dev:  ap3216c设备
 * @param - reg:  要读取的寄存器
 * @return 	  :   读取到的寄存器值
 */
static unsigned char ap3216c_read_reg(struct ap3216c_dev *dev, u8 reg)
{
    unsigned int data;
    int ret = 0;

    //ap3216c_read_regs(dev, reg, &data, 1);
    ret = regmap_read(dev->regmap, reg, &data);

    return data;
}

/*
 * @description	: 向ap3216c指定寄存器写入指定的值，写一个寄存器
 * @param - dev:  ap3216c设备
 * @param - reg:  要写的寄存器
 * @param - data: 要写入的值
 * @return   :    无
 */
static void ap3216c_write_reg(struct ap3216c_dev *dev, u8 reg, u8 data)
{
    int ret = 0;
    //ap3216c_write_regs(dev, reg, &data, 1);
    ret = regmap_write(dev->regmap, reg, data);
}

/*
 * @description	: 读取AP3216C的数据，读取原始数据，包括ALS,PS和IR, 注意！
 *				: 如果同时打开ALS,IR+PS的话两次数据读取的时间间隔要大于112.5ms
 * @param - ir	: ir数据
 * @param - ps 	: ps数据
 * @param - ps 	: als数据 
 * @return 		: 无。
 */
void ap3216c_readdata(struct ap3216c_dev *dev)
{
    unsigned char i =0;
    unsigned char buf[6];

    /* 循环读取所有传感器数据 */
    for(i = 0; i < 6; i++)
    {
        buf[i] = ap3216c_read_reg(dev, AP3216C_IRDATALOW + i);
    }

    if(buf[0] & 0X80) 	/* IR_OF位为1,则数据无效 */
		dev->ir = 0;					
	else 				/* 读取IR传感器的数据   		*/
		dev->ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 

    dev->als = ((unsigned short)buf[3] << 8) | buf[2];	/* 读取ALS传感器的数据 			 */

    if(buf[4] & 0x40)	/* IR_OF位为1,则数据无效 			*/
		dev->ps = 0;    													
	else 				/* 读取PS传感器的数据    */
		dev->ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);  
}
/**************************************************************************************/

/*******************************AP3216C操作函数********************************************/
static int ap3216c_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &ap3216cdev;
    /* 初始化AP3216C */
    ap3216c_write_reg(&ap3216cdev, AP3216C_SYSTEMCONG, 0x04);		/* 复位AP3216C 			*/
    mdelay(50);                                                     /* AP3216C复位最少10ms 	*/
    ap3216c_write_reg(&ap3216cdev, AP3216C_SYSTEMCONG, 0X03);		/* 开启ALS、PS+IR 		*/

    return 0;
}

static ssize_t ap3216c_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    short data[3];
	long err = 0;

    struct ap3216c_dev *dev = (struct ap3216c_dev *)filp->private_data;

    ap3216c_readdata(dev);      //读取传感器数据
    data[0] = dev->ir;
    data[1] = dev->als;
    data[2] = dev->ps;

    err = copy_to_user(buf, data, sizeof(data));

    return 0;
}

static int ap3216c_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* AP3216C操作函数 */
static const struct file_operations ap3216c_ops = {
	.owner = THIS_MODULE,
	.open = ap3216c_open,
	.read = ap3216c_read,
	.release = ap3216c_release,
};
/*****************************************************************************************/

 /*
  * @description     : i2c驱动的probe函数，当驱动与
  *                    设备匹配以后此函数就会执行
  * @param - client  : i2c设备
  * @param - id      : i2c设备ID
  * @return          : 0，成功;其他负值,失败
  */
static int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    /* 初始化regmap_config设置 */
	ap3216cdev.regmap_config.reg_bits = 8;		/* 寄存器长度8bit */
	ap3216cdev.regmap_config.val_bits = 8;		/* 值长度8bit */

    /* 初始化IIC接口的regmap */
	ap3216cdev.regmap = regmap_init_i2c(client, &ap3216cdev.regmap_config);
	if (IS_ERR(ap3216cdev.regmap)) {
		return  PTR_ERR(ap3216cdev.regmap);
	}

    /* 注册字符设备驱动 */
    /* 1、构建设备号 */
    if(ap3216cdev.major) {      //如果给定主设备号
        ap3216cdev.devid = MKDEV(ap3216cdev.major, 0);
        register_chrdev_region(ap3216cdev.devid, AP3216C_CNT, AP3216C_NAME);
    } else {
		alloc_chrdev_region(&ap3216cdev.devid, 0, AP3216C_CNT, AP3216C_NAME);
		ap3216cdev.major = MAJOR(ap3216cdev.devid);
	}

    /* 2、注册设备 */
    cdev_init(&ap3216cdev.cdev, &ap3216c_ops);
    cdev_add(&ap3216cdev.cdev, ap3216cdev.devid, AP3216C_CNT);

    /* 3、创建类 */
    ap3216cdev.class = class_create(THIS_MODULE, AP3216C_NAME);
    if (IS_ERR(ap3216cdev.class)) {
		return PTR_ERR(ap3216cdev.class);
	}

    /* 4、创建设备 */
    ap3216cdev.device = device_create(ap3216cdev.class, NULL, ap3216cdev.devid, NULL, AP3216C_NAME);
	if (IS_ERR(ap3216cdev.device)) {
		return PTR_ERR(ap3216cdev.device);
	}

    ap3216cdev.private_data = client;//把client存到全局变量ap3216cdev中，在ap3216c读写寄存器函数中可以取出client

    /* 保存ap3216cdev结构体 */
	i2c_set_clientdata(client, &ap3216cdev);

    return 0;
}

/*
 * @description     : i2c驱动的remove函数，移除i2c驱动的时候此函数会执行
 * @param - client 	: i2c设备
 * @return          : 0，成功;其他负值,失败
 */
static int ap3216c_remove(struct i2c_client *client)
{
    /* 删除设备 */
    cdev_del(&ap3216cdev.cdev);
	unregister_chrdev_region(ap3216cdev.devid, AP3216C_CNT);

    /* 注销掉类和设备 */
    device_destroy(ap3216cdev.class, ap3216cdev.devid);
	class_destroy(ap3216cdev.class);

    /* 释放regmap */
	regmap_exit(ap3216cdev.regmap);

    return 0;
}

/* 传统匹配方式ID列表 */
static struct i2c_device_id ap3216c_id[] = {
	{"alientek,ap3216c", 0},  
	{ /* Sentinel */ }
};

/* 设备树匹配表 */
struct of_device_id p3216c_of_match[] = {
    {.compatible = "alientek,ap3216c"},         //与设备树中i2c1节点下的ap3216c器件的compatible要对应
    { /* Sentinel */ }
};

/* i2c_driver结构体 */
static struct i2c_driver ap3216c_driver = {
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "ap3216c",                      //决定了驱动在 /sys/bus/i2c/drivers/ 目录下显示的文件夹名字
        .of_match_table = p3216c_of_match,      //用于和设备树中的compatible属性匹配
    },
    .id_table = ap3216c_id,                     //传统匹配方式,无设备树时与i2c_board_info进行匹配
};


/* 驱动入口函数 */
static int __init ap3216c_init(void)
{
    int ret = 0;

    /* 注册i2c_driver */
    ret = i2c_add_driver(&ap3216c_driver);

    return ret;
}

/* 驱动出口函数 */
static void __exit ap3216c_exit(void)
{
    /* 注销i2c_driver */
    i2c_del_driver(&ap3216c_driver);
}

/* 驱动模块的加载与卸载 */
module_init(ap3216c_init);
module_exit(ap3216c_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWY");