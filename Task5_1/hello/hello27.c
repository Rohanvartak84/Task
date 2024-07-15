/*most ioctl implementation consistt of big statemnet that selected correc behaviour 
 *
 * */

#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/kdev_t.h>	
#include <linux/printk.h>	
#include <linux/cdev.h>	
#include <linux/device.h>	
#include <linux/delay.h>	
#include <linux/fs.h>	
#include <linux/uaccess.h>	

int base_minor=0;
char *device_name = "msg27";
int count=1;
dev_t  dev_num;


MODULE_LICENSE("GPL");

static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;

#define MAX_SIZE 1024
char kernel_buffer[MAX_SIZE];
int buffer_index;


static int device_open(struct inode *inode, struct file *file)
{
	pr_info("%s\n",__func__);

	file->f_pos =0;
	buffer_index=0;
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	pr_info("%s\n",__func__);
	return 0;
}

static ssize_t device_read(struct file *file ,char __user *user_buffer,size_t read_count,loff_t *offset)
{	int bytes_read;
	int available_space;
	int bytes_to_read;
	
	pr_info("%s read offset %llu \n",__func__,*offset);
	available_space =MAX_SIZE - *(offset);
	
	if(read_count < available_space)
		bytes_to_read = read_count;
	else
		bytes_to_read = available_space;

	if(bytes_to_read == 0)
	{
	pr_err("%s: no availablr space inth buffer for readeing\n",__func__);
	return -ENOSPC;
	}

	if(buffer_index > *offset)
		bytes_to_read = buffer_index - *offset;
	else
		return 0;

	bytes_read = bytes_to_read - copy_to_user(user_buffer, kernel_buffer + *offset, bytes_to_read);

	pr_info("%s copy to user returned %d\n",__func__,bytes_to_read);
	*offset += bytes_read;
	return bytes_read;

}

static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t write_count,loff_t *offset)
{	
	int bytes_written;
	int available_space;
	int bytes_to_write;

	pr_info("%s write offset: %lld\n",__func__,*offset);
	available_space =MAX_SIZE - *(offset);
	
	if(write_count <available_space)
		bytes_to_write =write_count;
	else
		bytes_to_write = available_space;

	if(bytes_to_write == 0)
	{
	pr_err("%s : no available space in the buffer for writing\n",__func__);

	return -ENOSPC;
	}

		
	bytes_written = bytes_to_write -copy_from_user(kernel_buffer + *offset, user_buffer,bytes_to_write);
	pr_info("%s bytes to be written %s\n",__func__,kernel_buffer);
	pr_info("%s kernel_buffer  %s\n",__func__,kernel_buffer);

	*offset += bytes_written;
	buffer_index += bytes_written;
	return bytes_written;



}
long device_ioctl(struct file *filp, unsigned int cmd , unsigned long arg)
{

	unsigned char ch;
pr_info("%s :cmd %u \t arg%lu\n",__func__,cmd,arg);


switch(cmd)
{
//get length buffer
case 0x01:
	pr_info("get buffer length\n");
	put_user(MAX_SIZE,(unsigned int *)arg);
	break;

	//clear buffer
case 0x02:
	pr_info("clear buffer length\n");
	memset(kernel_buffer,0,sizeof(kernel_buffer));
	break;

case 0x03://fill character
	get_user(ch,(unsigned char*)arg);
	pr_info("fill character \n");
	memset(kernel_buffer,ch,sizeof(kernel_buffer));
	buffer_index =sizeof(kernel_buffer);
	break;

default:
	pr_info("unkoniwnd command %u\n",cmd);
	return -EINVAL;
}
return 0;

}

struct file_operations device_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.unlocked_ioctl =device_ioctl
};
static int test_hello26_init(void)
{
	
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

module_init(test_hello26_init);
module_exit(test_exit);

