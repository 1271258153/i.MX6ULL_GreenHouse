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

#define IRR_DEV_COUNT   1
#define IRR_DEV_NAME    "irr"

struct irr_dev{
    dev_t dev;
    u32 major;
    u32 minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *irr_node;
    int gpio_id;
    int error;
};
struct irr_dev irr;


int irr_open(struct inode *node, struct file *flop)
{
    flop->private_data = &irr;

    return 0;
}
ssize_t irr_read(struct file *flop, char __user *buf, size_t count, loff_t *loff)
{
    //struct irr_dev *irr = (struct irr_dev*)flop->private_data;



    return 0;
}
ssize_t irr_write(struct file *flop, const char __user *buf, size_t count, loff_t *loff)
{
    struct irr_dev *irr = (struct irr_dev*)flop->private_data;
    char irr_sta;
    irr->error = copy_from_user(&irr_sta,buf,count);
    if(irr->error < 0){
        printk("copy from user false!\n");
        return -1;
    }
    if(irr_sta == 1)
        gpio_set_value(irr->gpio_id,0); //开水泵
    else
        gpio_set_value(irr->gpio_id,1); //关水泵
    
    return 0;
}
int irr_release(struct inode *node, struct file *flop)
{



    return 0;
}

const struct file_operations flop = {
    .owner = THIS_MODULE,
    .open = irr_open,
    .write = irr_write,
    .read = irr_read,
    .release = irr_release,
};

static int irr_Init(void)
{
    /* 找到irr节点 */
    irr.irr_node = of_find_node_by_path("/imx_irr");
    if(irr.irr_node == NULL){
        printk("find irr node false!\n");
        goto irr_init_false;
    }

    /* 得到GPIO操作数 */
    irr.gpio_id = of_get_named_gpio(irr.irr_node,"gpio_irr",0);
    if(irr.gpio_id < 0){
        printk("get irr named fasle!\n");
        goto irr_init_false;
    }

    /* 请求GPIO */
    irr.error = gpio_request(irr.gpio_id,"imx_irr");
    if(irr.error < 0){
        printk("gpio request false!\n");
        goto irr_init_false;
    }

    /* 设置IO方向 */
    irr.error = gpio_direction_output(irr.gpio_id,1);
    if(irr.error < 0){
        printk("gpio direction false!\n");
        goto irr_init_false;
    }

    return 0;

irr_init_false:
    return -1;

}

static int module_enter(void)
{
    irr.major = 0;
    if(irr.major){
        irr.dev = MKDEV(irr.major,irr.minor);
        irr.error = register_chrdev_region(irr.dev, IRR_DEV_COUNT, IRR_DEV_NAME);
        if(irr.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }else{
        irr.error = alloc_chrdev_region(&irr.dev, 0, IRR_DEV_COUNT, IRR_DEV_NAME);
        if(irr.error < 0){
            printk("register chrdev false!\n");
            goto register_chrdev_false;
        }
    }

    irr.major = MAJOR(irr.dev);
    irr.minor = MINOR(irr.dev);
    printk("MAJOR = %d  MINOR = %d \n",irr.major,irr.minor);

    irr.cdev.owner = THIS_MODULE;
    cdev_init(&irr.cdev, &flop);
    irr.error = cdev_add(&irr.cdev, irr.dev,IRR_DEV_COUNT);
    if(irr.error < 0){
        printk("add cdev false!\n");
        goto add_cdev_false;
    }

    irr.class = class_create(THIS_MODULE,IRR_DEV_NAME);
    if(IS_ERR(irr.class)){
        printk("create clss false!\n");
        goto create_class_false;
    }
    irr.device = device_create(irr.class,NULL,irr.dev,NULL,IRR_DEV_NAME);
    if(IS_ERR(irr.device)){
        printk("create device false!\n");
        goto create_device_false;
    }

    irr.error = irr_Init();
    if(irr.error < 0){
        printk("irr init false!\n");
        goto irr_init_fasle;
    }

    printk("irr device init is OK!\n");

    return 0;

irr_init_fasle:
    device_destroy(irr.class,irr.dev);
create_device_false:
    class_destroy(irr.class);
create_class_false:
    cdev_del(&irr.cdev);
add_cdev_false:
    unregister_chrdev_region(irr.dev,IRR_DEV_COUNT);
register_chrdev_false:
    return -1;
}

static void module_export(void)
{
    cdev_del(&irr.cdev);
    unregister_chrdev_region(irr.dev,IRR_DEV_COUNT);
    device_destroy(irr.class,irr.dev);
    class_destroy(irr.class);
    gpio_free(irr.gpio_id);
    printk("irr exit!\n");
}

module_init(module_enter);
module_exit(module_export);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWY");
