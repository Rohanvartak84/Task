#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/kdev_t.h>	
#include <linux/printk.h>	
#include <linux/cdev.h>	
#include <linux/device.h>	
#include <linux/delay.h>	
#include <linux/fs.h>	
#include <linux/uaccess.h>
#include "mystruct.h"

int base_minor=0;
char *device_name = "mydevf11";
int count=1;
dev_t  dev_num;


MODULE_LICENSE("GPL");
static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;
struct file *file;
struct inode *inode;

abc kernel;

static int device_open(struct inode *inode, struct file *file)
{
	pr_info("%s\n",__func__);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	pr_info("%s\n",__func__);
	return 0;
}

static ssize_t device_read(struct file *file ,char __user *user_buffer,size_t count,loff_t *offset)
{
	pr_info("%s\n",__func__);
	int retval;
	kernel.i=99;
	kernel.c='x';
	retval=copy_to_user(user_buffer,&kernel,sizeof(kernel));
	pr_info("%s: copy to user returned: %d\n",__func__,retval);
        pr_info("%s: int: %d\t char: %c\t count: %lu\t offset: %llu\n",__func__,kernel.i,kernel.c,count,*offset);

	return 0;
}

static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t count,loff_t *offset)
{
	pr_info("%s\n",__func__);
	int retval;
	retval=copy_from_user(&kernel,user_buffer,count);
	pr_info("%s: copy from user returned: %d\n",__func__,retval);
	pr_info("%s: int: %d\t char: %c\t count: %lu\t offset: %llu\n",__func__,kernel.i,kernel.c,count,*offset);

	return count;
}

struct file_operations device_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};
static int test_hello5_init(void)
{
	//dev1 = MKDEV(200,1);
	//dev2 = MKDEV(200,2);
	class=class_create("myclass6a");
	if(!alloc_chrdev_region(&dev_num,base_minor,count,device_name)){
		printk("Device number registerd\n");
		printk("Major number received:%d\n",MAJOR(dev_num));
		device=device_create(class,NULL,dev_num,NULL,device_name);
		cdev_init(&mycdev,&device_fops);
		mycdev.owner = THIS_MODULE;
		cdev_add(&mycdev,dev_num,count);
	}
	else
		printk("Device number not regestered\n");
	return 0;
}

static void test_exit(void)
{
	unregister_chrdev_region(dev_num,count);
	cdev_del(&mycdev);
	device_destroy(class,dev_num);
	class_destroy(class);
	printk("Driver unloaded\n");
}

module_init(test_hello5_init);
module_exit(test_exit);
