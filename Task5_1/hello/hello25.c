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

#define MAX_DEVICES 5

int baseminor=0;
int count =1;
char *devicename="mydevh25";
dev_t devicenumber;

static struct class *class=NULL;
static struct device *device=NULL;
//static struct cdev mycdev[MAX_DEVICES];

#define MAX_SIZE 1024
struct msg_device{
	struct cdev mycdev;
	char kernel_buffer[MAX_SIZE];
};

static struct msg_device msg_devices[MAX_DEVICES];
int buffer_index;



static int device_open(struct inode *inode,struct file *file){
	struct msg_device *my_device = container_of(inode->i_cdev, struct msg_device, mycdev);
	file->private_data = my_device;

	pr_info("%s\n",__func__);
	return 0;
}

static int device_release(struct inode *inode,struct file *file){
	pr_info("%s\n",__func__);
	return 0;
}

static ssize_t device_read(struct file *file,char __user *user_buffer,size_t read_count,loff_t *offset){
	pr_info("%s\n",__func__);
	
	int bytes_read;
        int available_space;
        int bytes_to_read;
	
	struct msg_device *my_device = (struct msg_device *)file->private_data;
	//int retval;
	pr_info("%s read offset:%lld\n",__func__,*offset);


	available_space=MAX_SIZE-*(offset);

	if(read_count<available_space)
		bytes_to_read=read_count;
	else
		bytes_to_read=available_space;
	
	
	if(bytes_to_read == 0){
		pr_err("%s: no available space in the buffer for reading\n",__func__);
		return -ENOSPC;
	}
	
	if(buffer_index>*offset)
		bytes_to_read=buffer_index - *offset;
	else
		return 0;

	bytes_read=bytes_to_read - copy_to_user(user_buffer,my_device->kernel_buffer+*offset,bytes_to_read);
	pr_info("%s: copy to user returned: %d\n",__func__,bytes_to_read);
	
	*offset += bytes_read;

	return bytes_read;
}


static ssize_t device_write(struct file *file ,const char __user *user_buffer,size_t write_count,loff_t *offset){
	pr_info("%s\n",__func__);
	
	int bytes_written;
	int available_space;
	int bytes_to_write;

	struct msg_device *my_device = (struct msg_device *)file->private_data;
        //int retval;

        pr_info("%s write offset:%lld\n",__func__,*offset);
       
	available_space=MAX_SIZE-*(offset);

        if(write_count<available_space)
                bytes_to_write=write_count;
        else
                bytes_to_write=available_space;


        if(bytes_to_write == 0){
                pr_err("%s: no available space in the buffer for writing\n",__func__);
                return -ENOSPC;
        }
       
	bytes_written=bytes_to_write - copy_from_user(my_device->kernel_buffer+*offset,user_buffer,bytes_to_write);
	pr_info("%s: bytes written: %d\n",__func__,bytes_written);
	pr_info("%s: kernel buffer: %s\n",__func__,my_device->kernel_buffer);


	*offset += bytes_written;
	buffer_index += bytes_written;
	return bytes_written;
}

static loff_t device_lseek(struct file *file,loff_t offset,int orig){
	loff_t new_pos=0;

	switch(orig){
		case 0: new_pos=offset;
			break;
		case 1: new_pos=file->f_pos + offset;
			break;
		case 2: new_pos=MAX_SIZE - offset;
			break;
	}
	if(new_pos>MAX_SIZE)
		new_pos=MAX_SIZE;
	if(new_pos <0)
		new_pos=0;
	file->f_pos = new_pos;
	return new_pos;
}



struct file_operations device_fops ={
	.read=device_read,
	.write=device_write,
	.open=device_open,
	.release=device_release,
	.llseek = device_lseek
};


static int fops_init(void){
	int i=0;
	class=class_create("myclass");
	if(!alloc_chrdev_region(&devicenumber,baseminor,count,devicename)){
		int major = MAJOR(devicenumber);
		pr_info("device registered\n");
		pr_info("major number recieved: %d\n",major);
		for(i=0;i<MAX_DEVICES;i++){
			dev_t tmp_device = MKDEV(major,i);
			device=device_create(class,NULL,tmp_device,NULL,"%s%d",devicename,i);
			cdev_init(&msg_devices[i].mycdev,&device_fops);
			msg_devices[i].mycdev.owner=THIS_MODULE;
			cdev_add(&msg_devices[i].mycdev,tmp_device,1);
	}
	}
	else{
		pr_info("device not registered\n");
	}
return 0;
}

static void fops_exit(void){
	int major=MAJOR(devicenumber);
	int i=0;
	dev_t tmp_device;
	for(i=0;i<MAX_DEVICES;i++){
		tmp_device = MKDEV(major,i);
		device_destroy(class,tmp_device);
		cdev_del(&msg_devices[i].mycdev);
	}
	class_destroy(class);
	unregister_chrdev_region(devicenumber,count);
	pr_info("module removed\n");
}

module_init(fops_init);
module_exit(fops_exit);


