#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/kdev_t.h>	
#include <linux/printk.h>	
#include <linux/cdev.h>	
#include <linux/device.h>	
#include <linux/delay.h>	
#include <linux/fs.h>	
#include <linux/uaccess.h>	

MODULE_LICENSE("GPL");

int baseminor=0;
int count =1;
char *devicename="mydevf8";
dev_t devicenumber;

static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;

static int device_open(struct inode *inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}

static int device_release(struct inode *inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}

static ssize_t device_read(struct file *file,char __user *user_buffer,size_t count,loff_t *offset){
	pr_info("%s\n",__func__);
	int num=12345;

	if(!put_user(num,(u64*)user_buffer))
		return sizeof(num);

	return 0;
}


static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t count,loff_t *offset){
	pr_info("%s\n",__func__);
	return count;

}

struct file_operations device_fops ={
	.read=device_read,
	.write=device_write,
	.open=device_open,
	.release=device_release
};


static int fops_init(void){
	class=class_create("myclass");
	if(!alloc_chrdev_region(&devicenumber,baseminor,count,devicename)){
		pr_info("device registered\n");
		pr_info("major number recieved: %d\n",MAJOR(devicenumber));
		device=device_create(class,NULL,devicenumber,NULL,devicename);
		cdev_init(&mycdev,&device_fops);
		mycdev.owner=THIS_MODULE;
		cdev_add(&mycdev,devicenumber,count);
	}
	else{
		pr_info("device not registered\n");
	}
return 0;
}

static void fops_exit(void){
	unregister_chrdev_region(devicenumber,count);
	cdev_del(&mycdev);
	device_destroy(class,devicenumber);
	class_destroy(class);
	pr_info("module removed\n");
}

module_init(fops_init);
module_exit(fops_exit);


