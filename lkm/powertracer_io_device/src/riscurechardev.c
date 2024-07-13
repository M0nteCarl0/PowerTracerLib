
/*
	author: Molotaliev Alexanderru

	date: 13.08.2023

	DESCRIPTION
		Riscure char device
*/

	// at least for empty module
#include <linux/module.h>
#include<linux/moduleparam.h>// module_param()
#include <linux/kernel.h> // for printk()
#include <linux/init.h>

	// at least for char device
#include<linux/fs.h> // struct file_operations
#include<linux/uaccess.h> // copy_to_user()
#include<linux/cdev.h>
#include<linux/device.h>// device_create()

#define DEVICE_NAME "riscure_device"
#define DEVICE_FIRST 0
#define DEVICE_COUNT 1

#define FIFO_SIZE 1024

	// for char device
static int dev_open(struct inode * n, struct file * f);
static int dev_release(struct inode * n, struct file * f);
static ssize_t dev_read(struct file * f,char __user * buf, size_t count, loff_t * ppos);
static ssize_t dev_write(struct file * f,const char __user * buf,size_t count,loff_t * ppos);

static const struct file_operations dev_fops =
{
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write
};

	// module parameters
static int major = 0;//100;
module_param(major,int,0);

static int dev_major = 0;
static struct cdev mod_cdev;
static struct class *dev_class;

static int major_number;
static int read_index;
static int write_index;
static char fifo[FIFO_SIZE];


	// module mandatory functions
static int __init mod_init(void)
{
			// register module as device
	int ret;
	dev_t dev;

		// get/set major
	if(major == 0) // get dev_major automatically
	{
		ret = alloc_chrdev_region(&dev,DEVICE_FIRST,DEVICE_COUNT,DEVICE_NAME);

		dev_major = MAJOR(dev);
	}
	else // set dev_major manually
	{
		dev_major = major;

		dev = MKDEV(dev_major,DEVICE_FIRST);
		ret = register_chrdev_region(dev,DEVICE_COUNT,DEVICE_NAME);
	}

	if(ret < 0)
	{
		printk(KERN_ERR"riscure_device: can not register char device region\n");
		return ret;
	}

		// init cdev structure
	cdev_init(&mod_cdev,&dev_fops);
	mod_cdev.owner = THIS_MODULE;
	mod_cdev.ops = &dev_fops;

		// add char device to kernel
	ret = cdev_add(&mod_cdev,dev,DEVICE_COUNT);

	if(ret < 0)
	{
		unregister_chrdev_region(MKDEV(dev_major,DEVICE_FIRST),DEVICE_COUNT);
		printk(KERN_ERR "riscure_device: can not add char device\n");
		return ret;
	}

		// create device file in directory /dev
	dev_class = class_create(THIS_MODULE,"dyn_class");
	device_create(dev_class,NULL,dev,"%s",DEVICE_NAME);

		// log
	printk(KERN_INFO "riscure_device: module installed\n");

	return ret;
}

static void __exit mod_exit(void)
{
		// delete device file from directory /dev
	device_destroy(dev_class,MKDEV(dev_major,DEVICE_FIRST));
	class_destroy(dev_class);

	cdev_del(&mod_cdev);
	unregister_chrdev_region(MKDEV(dev_major,DEVICE_FIRST),DEVICE_COUNT);

		// log
	printk(KERN_INFO "riscure_device: module removed\n");
}

module_init(mod_init);
module_exit(mod_exit);

	// info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Molotaliev Alexander");
MODULE_DESCRIPTION("Riscure power tracer device emulator");
MODULE_PARM_DESC(major,"0.1");

	// for char device
static int dev_open_count = 0;// reference counter

static int dev_open(struct inode * n, struct file * f)
{
	if(dev_open_count)
		return -EBUSY;

	dev_open_count += 1;

	return 0;
}

static int dev_release(struct inode * n, struct file * f)
{
	if(dev_open_count)
		dev_open_count -= 1;

	return 0;
}

static ssize_t dev_read(struct file * f,char __user * buf, size_t count, loff_t * ppos)
{
	int bytes_read = 0;

    if (read_index == write_index)
        return 0;

    while (count && read_index != write_index) {
        put_user(fifo[read_index], buf);
        buf++;
        bytes_read++;
        read_index = (read_index + 1) % FIFO_SIZE;
        count--;
    }

    return bytes_read;
}

#define MSG_LEN 64
static char write_info[MSG_LEN + 1] = "info";

static ssize_t dev_write(struct file * f,const char __user * buf,size_t count,loff_t * ppos)
{

	int bytes_written = 0;
    while (count && (write_index + 1) % FIFO_SIZE != read_index) {
        get_user(fifo[write_index], buf);
        buf++;
        bytes_written++;
        write_index = (write_index + 1) % FIFO_SIZE;
        count--;
    }
    return bytes_written;
}



