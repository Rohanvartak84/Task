#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/printk.h>
#include<linux/kernel.h>
#include<linux/delay.h>
#include<linux/fs.h>



MODULE_LICENSE("GPL");
int base_minor=0;
int count=1;
char *device_name="rohandevice";
dev_t devicenumber;
static struct class *class;
static struct device *device;


static int device_open(struct inode*inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}
static int device_release(struct inode*inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}


static int device_read(struct file *file,char __user *user_buffer,size_t count, loff_t *offset){
	pr_info("%s\n",__func__);
	return 0;
}

static int device_write(struct file *file,char __user *user_buffer,size_t count, loff_t *offset){
	pr_info("%s\n",__func__);
	return count;
}


struct file_operations device_fops = {
	.read = device_read;
	.write = device_write;
	.open = device_open;
	.release = device_release
};


static int test_start(void){

	class= class_create("myclass");

	if(!alloc_chrdev_region(&devicenumber,base_minor,count,device_name)){
		printk("device registered\n");
		printk("major num: %d\n",MAJOR(devicenumber));
		device= device_create(class,NULL,devicenumber,NULL,device_name);
	}
	else
		printk("device not registered\n");
}

static void test_stop(void){
	unregistered_chrdev_region(devicenumber,count);
	device_destroy(class,devicenumber);
	class_destroy(class);
	printk("device destroyed\n");
}

module_init(test_start);
module_exit(test_stop);

