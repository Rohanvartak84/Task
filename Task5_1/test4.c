#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/printk.h>
#include<linux/kernel.h>
#include<linux/delay.h>



MODULE_LICENSE("GPL");
int base_minor=0;
int count=1;
char *device_name="rohandevice";
dev_t devicenumber;


static struct class *class;
static struct device *device;

static int test_start(void){

	class= class_create("rohanclass");

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

