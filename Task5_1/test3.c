#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/printk.h>
#include<linux/kernel.h>
#include<linux/delay.h>



MODULE_LICENSE("GPL");
static struct class *class;
static struct device *device1,*device2;
dev_t dev1,dev2;

static int test_start(void){
	dev1=MKDEV(200,1);
	dev2=MKDEV(200,2);

	//class= class_create(THIS_MODULE, "myclass4");
	class= class_create("myclass4");
	device1= device_create(class,NULL,dev1,NULL,"mydevice%d",1);
	device2= device_create(class,NULL,dev2,NULL,"mydevice%d",2);
	printk("device created\n");
	return 0;
}

static void test_stop(void){
	device_destroy(class,dev1);
	device_destroy(class,dev2);
	class_destroy(class);
	printk("device destroyed\n");
}

module_init(test_start);
module_exit(test_stop);

