#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/printk.h>
#include<linux/init.h>

int major_num=120;
int minor_num=0;

char *d_name="mychardev";
int count =1;
dev_t dnum;

module_param(major_num,int,0);
module_param(minor_num,int,0);
module_param(count,int,0);
module_param(d_name,charp,0);

MODULE_LICENSE("GPL");

static int test_start(void){

        printk("module loaded\n");
        dnum=MKDEV(major_num,minor_num);
        printk("major num: %d\n",MAJOR(dnum));
        printk("minor num: %d\n",MINOR(dnum));
        printk("count: %d\n",count);
        printk("device name: %s\n",d_name);

        if(!register_chrdev_region(dnum,count,d_name)){
                printk("device number registered\n");
        }
        else
                printk("device number registration failed\n");

return 0;
}

static void test_stop(void){
	unregister_chrdev_region(dnum, 1);
        printk("module exit\n");
}

module_init(test_start);
module_exit(test_stop);

