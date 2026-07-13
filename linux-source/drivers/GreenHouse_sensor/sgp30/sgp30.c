#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include "sgp30.h"

#define SGP30_DEV_COUNT   1
#define SGP30_DEV_NAME    "sgp30"

static int sgp30_probe(struct i2c_client *client, const struct i2c_device_id *dev_id);
static int sgp30_remove(struct i2c_client *cliet);
int sgp30_open(struct inode *node, struct file *flop);
ssize_t sgp30_read(struct file *flop, char __user *buf, size_t count, loff_t *loff);
int sgp30_release(struct inode *node, struct file *flop);
int sgp30_init(void);
u16 sgp30_read_data(void);

struct sgp30_dev{
    dev_t dev;
    u32 major;
    u32 minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    void *private_data; 
    int error;
};
struct sgp30_dev sgp30;

struct i2c_device_id sgp30_id[] = {
    {"sgp30",0},
    {}
};

const struct of_device_id sgp30_match_table[] = {
    {.compatible = "sgp30"},
    {}
};

struct i2c_driver sgp30_driver = {
    .id_table = sgp30_id,
    .driver = {
        .owner = THIS_MODULE,
        .name = "lsc_sgp30",
        .of_match_table = of_match_ptr(sgp30_match_table),
    },
    .probe = sgp30_probe,
    .remove = sgp30_remove,
};

const struct file_operations flop = {
    .owner = THIS_MODULE,
    .open = sgp30_open,
    .read = sgp30_read,
    .release = sgp30_release,
};

int sgp30_open(struct inode *node, struct file *flop)
{
    flop->private_data = &sgp30;

    return 0;
}
ssize_t sgp30_read(struct file *flop, char __user *buf, size_t count, loff_t *loff)
{
    u16 CO2;
    int ret = 0;
    CO2 = sgp30_read_data();
    ret = copy_to_user((u16 *)buf,&CO2,count);
    if(ret < 0){
        printk("copy to user err\n");
    }

    return 0;
}

int sgp30_release(struct inode *node, struct file *flop)
{
    return 0;
}

static int sgp30_read_datas(struct sgp30_dev *dev,void *val,int len)
{
    struct i2c_client *client = (struct i2c_client *)dev->private_data;
    struct i2c_msg msgs;

    msgs.addr = client->addr;
    msgs.flags = 1;
    msgs.buf = val;
    msgs.len = len;

    return i2c_transfer(client->adapter, &msgs, 1);
}

static int sgp30_write_datas(struct sgp30_dev *dev,u8 *buf,int len)
{
    struct i2c_client *client = (struct i2c_client *)dev->private_data;
    struct i2c_msg msgs;

    msgs.addr = client->addr;
    msgs.flags = 0;
    msgs.buf = buf;
    msgs.len = len;

    return i2c_transfer(client->adapter, &msgs, 1);
}

int sgp30_init(void)
{
    int ret = 0;
    u8 buf[2];
    buf[0] = Init_air_quality >> 8;     // 高8位
    buf[1] = Init_air_quality & 0xff;   // 低8位

    /* 发送初始化命令 Init_air_quality = 0x2003 */
    ret = sgp30_write_datas(&sgp30,buf,2);
    /* 等待 100ms */
    mdelay(100);
    return ret;
}

u16 sgp30_read_data(void)
{
    u16 CO2 = 0;
    u8 r_data[5] = {0};
    u8 buf[2];
    buf[0] = Measure_air_quality >> 8;
    buf[1] = Measure_air_quality & 0xff;

    /* 发送测量命令 Measure_air_quality = 0x2008 */
    sgp30_write_datas(&sgp30,buf,2);
    mdelay(100);

    /* 读取测量数据 */
    sgp30_read_datas(&sgp30,r_data,5);
    CO2 = ((u16)r_data[0] << 8) | r_data[1];
    return CO2;
}

static int sgp30_probe(struct i2c_client *client, const struct i2c_device_id *dev_id)
{
    int ret = 0;
    u16 CO2_val;
    sgp30.private_data = client;
    
    sgp30.major = 0;
    if(sgp30.major){
        sgp30.dev = MKDEV(sgp30.major,sgp30.minor);
        sgp30.error = register_chrdev_region(sgp30.dev, SGP30_DEV_COUNT, SGP30_DEV_NAME);
        if(sgp30.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }else{
        sgp30.error = alloc_chrdev_region(&sgp30.dev, 0, SGP30_DEV_COUNT, SGP30_DEV_NAME);
        if(sgp30.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }

    /* 获取主设备号和次设备号 */
    sgp30.major = MAJOR(sgp30.dev);
    sgp30.minor = MINOR(sgp30.dev);
    printk("MAJOR = %d  MINOR = %d \n",sgp30.major,sgp30.minor);

    sgp30.cdev.owner = THIS_MODULE;
    cdev_init(&sgp30.cdev, &flop);
    sgp30.error = cdev_add(&sgp30.cdev, sgp30.dev,SGP30_DEV_COUNT);
    if(sgp30.error < 0){
        printk("add cdev false!\n");
        goto add_cdev_false;
    }

    sgp30.class = class_create(THIS_MODULE,SGP30_DEV_NAME);
    if(IS_ERR(sgp30.class)){
        printk("create clss false!\n");
        goto create_class_false;
    }
    sgp30.device = device_create(sgp30.class,NULL,sgp30.dev,NULL,SGP30_DEV_NAME);
    if(IS_ERR(sgp30.device)){
        printk("create device false!\n");
        goto create_device_false;
    }

    /* 初始化 SGP30 */
    ret = sgp30_init();
    if(ret < 0){
        printk("init SGP30 false!\n");
        goto create_device_false;
    }

    /* 读取 SGP30 数据 */
    CO2_val = sgp30_read_data();
    if(CO2_val == 400){
        printk("SGP30 is OK!\n");
    }
    return 0;

create_device_false:
    class_destroy(sgp30.class);
create_class_false:
    cdev_del(&sgp30.cdev);
add_cdev_false:
    unregister_chrdev_region(sgp30.dev,SGP30_DEV_COUNT);
register_chrdev_false:
    return -1;
}

static int sgp30_remove(struct i2c_client *cliet)
{
    cdev_del(&sgp30.cdev);
    unregister_chrdev_region(sgp30.dev,SGP30_DEV_COUNT);
    device_destroy(sgp30.class,sgp30.dev);
    class_destroy(sgp30.class);

    return 0;
}

static int module_enter(void)
{
    int ret = 0;

    ret = i2c_add_driver(&sgp30_driver);
    if(ret < 0){
        printk("add driver false\n");
    }

    return 0;
}

static void module_export(void)
{
    i2c_del_driver(&sgp30_driver);
    printk("module export\n");
}

module_init(module_enter);
module_exit(module_export);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWY");
