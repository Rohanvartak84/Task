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
char *devicename="mydevh22";
dev_t devicenumber;

static struct class *class=NULL;
static struct device *device=NULL;
static struct cdev mycdev;

#define MAX_SIZE 1024
char kernel_buffer[MAX_SIZE];
int buffer_index;



static int device_open(struct inode *inode,struct file *file){
	pr_info("%s\n",__func__);
	file->f_pos=0;
	buffer_index=0;
	return 0;
}

static int device_release(struct inode *inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}

static ssize_t device_read(struct file *file,char __user *user_buffer,size_t read_count,loff_t *offset){
	nt bytes_read;
        int available_space;
        int bytes_to_read;
	
	pr_info("%s\n",__func__);
	int retval;
	pr_info("%s read offset:%lld\n",__func__,*offset);


	availabe_space=MAX_SIZE-*(offset);

	if(read_count<availabe_space)
		bytes_to_read=read_count;
	else
		bytes_to_read=availabe_space;
	
	
	if(bytes_to_read == 0){
		pr_err("%s: no available space in the buffer for reading\n",__func__);
		return -ENOSPC;
	}
	
	if(buffer_index>*offset)
		bytes_to_read=buffer_index - *offset;
	else
		return 0;

	bytes_read=bytes_to_read - copy_to_user(user_buffer,kernel_buffer+*offset,bytes_to_read);
	pr_info("%s: copy to user returned: %d\n",__func__,bytes_to_read);
	
	*offset += bytes_read;

	return bytes_read;
}


static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t write_count,loff_t *offset){
	int bytes_written;
	int available_space;
	int bytes_to_write;

	pr_info("%s\n",__func__);
        int retval;

        pr_info("%s write offset:%lld\n",__func__,*offset);
       
	availabe_space=MAX_SIZE-*(offset);

        if(write_count<availabe_space)
                bytes_to_write=write_count;
        else
                bytes_to_write=availabe_space;


        if(bytes_to_write == 0){
                pr_err("%s: no available space in the buffer for writing\n",__func__);
                return -ENOSPC;
        }
       
	bytes_written=bytes_to_write - copy_from_user(kernel_buffer+*offset,user_buffer,bytes_to_write);
	pr_info("%s: bytes written: %d\n",__func__,bytes_written);
	pr_info("%s: kernel buffer: %d\n",__func__,kernel_buffer);


	*offset += bytes_written;
	buffer_index += bytes_written;
	return bytes_written;
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


