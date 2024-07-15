#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/kdev_t.h>	
#include <linux/printk.h>	
#include <linux/cdev.h>	
#include <linux/device.h>	
#include <linux/delay.h>	
#include <linux/fs.h>	

int base_minor=0;
char *device_name = "mydevf4";
int count=1;
dev_t  dev_num;

MODULE_LICENSE("GPL");
static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;
struct file *file;
struct inode *inode;

static int device_open(struct inode *inode, struct file *file)
{
	pr_info("%s\n",__func__);
	
	if((file->f_flags&O_ACCMODE) == O_RDONLY) {
		pr_info("opened file in read only\n");
	}
	else if((file->f_flags&O_ACCMODE) == O_WRONLY) {
		pr_info("opened file in write only\n");
	}
	else if((file->f_flags&O_ACCMODE) == O_RDWR) {
		pr_info("opened file in Read/write mode\n");
	}
	if(file->f_flags & O_CREAT) {
		pr_info("create file if does not exist\n");
	}
	if(file->f_flags & O_EXCL) {
		pr_info("Provide exclusive access\n");
	}
	if(file->f_flags & O_TRUNC) {
		pr_info("Truncate the file to zero size first\n");
	}
	if(file->f_flags & O_APPEND) {
		pr_info("Append to the file (dont overwrite)\n");
	}
	if(file->f_flags & O_NONBLOCK) {
		pr_info("Access methods are non-blocking\n");
	}
	if(file->f_flags & O_SYNC) {
		pr_info("O_SYNC\n");
	}

	pr_info("file offset:%llu\n",file->f_pos);

	pr_info("Mode:%ho\n",inode->i_mode);
	pr_info("User Id:%d\n",inode->i_uid.val);
	pr_info("Group Id:%d\n",inode->i_gid.val);
	pr_info("Inode number:%ld\n",inode->i_ino);
	pr_info("Major number:%d\t Minor Number:%d\n:",imajor(inode),iminor(inode));
	pr_info("Major number:%d\t Minor Number:%d\n:",MAJOR(inode->i_rdev),MINOR(inode->i_rdev));
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
	return 0;
}

static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t count,loff_t *offset)
{
	pr_info("%s\n",__func__);
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
