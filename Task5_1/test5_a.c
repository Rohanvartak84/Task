#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include<linux/delay.h>
#include<linux/cdev.h>
#include<linux/device.h>
int count=1;
char *device_name="mydevice6a";
static int base_minor=0;
static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;
dev_t devicenumber;
MODULE_LICENSE("GPL");
static int device_open(struct inode *inode,struct file*file)
{
        pr_info("%s\n",__func__);
        return 0;
}
static int device_release(struct inode *inode,struct file*file)
{
        pr_info("%s\n",__func__);
        return 0;
}
 
static ssize_t  device_read(struct file*file,char __user *user_buffer,size_t count,loff_t*offset)
{
        pr_info("%s\n",__func__);
        return 0;
}
static ssize_t  device_write(struct file*file,const char __user *user_buffer,size_t count,loff_t*offset)
{
        pr_info("%s\n",__func__);
        return count;
}
 
struct file_operations device_fops={
        .read=device_read,
        .write=device_write,
        .open=device_open,
        .release=device_release
};
static int test_hello6a_init(void)
{
        class = class_create("myclass");
        printk("device loaded\n");
        if(!alloc_chrdev_region(&devicenumber,base_minor,count,device_name))
        {
                printk("Device created\n");
                printk("major number :%d\n",MAJOR(devicenumber));
                device = device_create(class,NULL,devicenumber,NULL,device_name);
                cdev_init(&mycdev,&device_fops);
                mycdev.owner=THIS_MODULE;
                cdev_add(&mycdev,devicenumber,count);
 
        }
        else
        {
                printk("Device is not loaded\n");
        }
        return 0;
}
static void test_hello6a_exit(void)
{
unregister_chrdev_region(devicenumber,count);
device_destroy(class,devicenumber);
class_destroy(class);
cdev_del(&mycdev);
 
printk("device deatored\n");
 
}
module_init(test_hello6a_init);
module_exit(test_hello6a_exit);
