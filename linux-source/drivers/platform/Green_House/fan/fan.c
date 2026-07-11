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

#define FAN_DEV_COUNT   1
#define FAN_DEV_NAME    "fan"

struct fan_dev{
    dev_t dev;
    u32 major;
    u32 minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *fan_node;
    int gpio_id;
    int error;
};
struct fan_dev fan;


int fan_open(struct inode *node, struct file *flop)
{
    flop->private_data = &fan;

    return 0;
}
ssize_t fan_read(struct file *flop, char __user *buf, size_t count, loff_t *loff)
{
    //struct fan_dev *fan = (struct fan_dev*)flop->private_data;

    return 0;
}
ssize_t fan_write(struct file *flop, const char __user *buf, size_t count, loff_t *loff)
{
    struct fan_dev *fan = (struct fan_dev*)flop->private_data;
    char fan_sta;
    fan->error = copy_from_user(&fan_sta,buf,count);
    if(fan->error < 0){
        printk("copy from user false!\n");
        return -1;
    }
    if(fan_sta == 1)
        gpio_set_value(fan->gpio_id,0); //转
    else
        gpio_set_value(fan->gpio_id,1); //停
    
    return 0;
}
int fan_release(struct inode *node, struct file *flop)
{

    return 0;
}

const struct file_operations flop = {
    .owner = THIS_MODULE,
    .open = fan_open,
    .write = fan_write,
    .read = fan_read,
    .release = fan_release,
};

static int fan_Init(void)
{
    /* 找到fan节点 */
    fan.fan_node = of_find_node_by_path("/imx_fan");
    if(fan.fan_node == NULL){
        printk("find fan node false!\n");
        goto fan_init_false;
    }

    /* 得到GPIO操作数 */
    fan.gpio_id = of_get_named_gpio(fan.fan_node,"gpio_fan",0);
    if(fan.gpio_id < 0){
        printk("get fan named fasle!\n");
        goto fan_init_false;
    }

    /* 请求GPIO */
    fan.error = gpio_request(fan.gpio_id,"imx_fan");
    if(fan.error < 0){
        printk("gpio request false!\n");
        goto fan_init_false;
    }

    /* 设置IO方向 */
    fan.error = gpio_direction_output(fan.gpio_id,1);
    if(fan.error < 0){
        printk("gpio direction false!\n");
        goto fan_init_false;
    }

    return 0;

fan_init_false:
    return -1;

}

static int module_enter(void)
{
    fan.major = 0;
    if(fan.major){
        fan.dev = MKDEV(fan.major,fan.minor);
        fan.error = register_chrdev_region(fan.dev, FAN_DEV_COUNT, FAN_DEV_NAME);
        if(fan.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }else{
        fan.error = alloc_chrdev_region(&fan.dev, 0, FAN_DEV_COUNT, FAN_DEV_NAME);
        if(fan.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }

    fan.major = MAJOR(fan.dev);
    fan.minor = MINOR(fan.dev);
    printk("MAJOR = %d  MINOR = %d \n",fan.major,fan.minor);

    fan.cdev.owner = THIS_MODULE;
    cdev_init(&fan.cdev, &flop);
    fan.error = cdev_add(&fan.cdev, fan.dev,FAN_DEV_COUNT);
    if(fan.error < 0){
        printk("add cdev false!\n");
        goto add_cdev_false;
    }

    fan.class = class_create(THIS_MODULE,FAN_DEV_NAME);
    if(IS_ERR(fan.class)){
        printk("create clss false!\n");
        goto create_class_false;
    }
    fan.device = device_create(fan.class,NULL,fan.dev,NULL,FAN_DEV_NAME);
    if(IS_ERR(fan.device)){
        printk("create device false!\n");
        goto create_device_false;
    }

    fan.error = fan_Init();
    if(fan.error < 0){
        printk("fan init false!\n");
        goto fan_init_fasle;
    }

    printk("fan device init is OK!\n");

    return 0;

fan_init_fasle:
    device_destroy(fan.class,fan.dev);
create_device_false:
    class_destroy(fan.class);
create_class_false:
    cdev_del(&fan.cdev);
add_cdev_false:
    unregister_chrdev_region(fan.dev,FAN_DEV_COUNT);
register_chrdev_false:
    return -1;
}

static void module_export(void)
{
    cdev_del(&fan.cdev);
    unregister_chrdev_region(fan.dev,FAN_DEV_COUNT);
    device_destroy(fan.class,fan.dev);
    class_destroy(fan.class);
    gpio_free(fan.gpio_id);
    printk("fan exit!\n");
}

module_init(module_enter);
module_exit(module_export);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWY");
