#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/printk.h>
#include<linux/init.h>

int base_minor=0;
int major_num=120;
int minor_num=0;

char *d_name="mychardev";
int count =1;
dev_t dnum;

module_param(base_minor,int,0);
//module_param(major_num,int,0);
//module_param(minor_num,int,0);
module_param(count,int,0);
module_param(d_name,charp,0);

MODULE_LICENSE("GPL");

static int test_start(void){

        printk("module loaded\n");
        //dnum=MKDEV(major_num,minor_num);
        //printk("major num: %d\n",MAJOR(dnum));
        printk("minor num: %d\n",base_minor);
        printk("count: %d\n",count);
        printk("device name: %s\n",d_name);

        if(!alloc_chrdev_region(&dnum,base_minor,count,d_name)){
                printk("device number registered\n");
		printk("major number recieved: %d\n",MAJOR(dnum));
        }
        else
                printk("device number registration failed\n");

return 0;
}

static void test_stop(void){
	unregister_chrdev_region(dnum,count);
        printk("module exit\n");
}

module_init(test_start);
module_exit(test_stop);

