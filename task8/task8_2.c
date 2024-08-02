#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

static struct kobject *test_kobj;
static int number;

static ssize_t number_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%d\n", number);
}

static ssize_t number_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	sscanf(buf, "%d", &number);
	return count;
}

static struct kobj_attribute number_attribute = __ATTR(number, 0664, number_show, number_store);

static int __init sysfs_test_init(void) {
	int error;
	
	pr_info("kernel module inserted\n");
	// Create a kobject in the /sys directory
	test_kobj = kobject_create_and_add("test",test_kobj);
	if (!test_kobj) {
		printk(KERN_ALERT "Could not create kobject in /sys!\n");
		return -ENOMEM;
	}

	// Create the file associated with this kobject
	error = sysfs_create_file(test_kobj, &number_attribute.attr);
	if (error) {
		printk(KERN_ALERT "Could not create sysfs file!\n");
		kobject_put(test_kobj);
		return error;
	}

	return 0;
}

static void __exit sysfs_test_exit(void) {
	pr_info("kernel module released\n");
	sysfs_remove_file(test_kobj, &number_attribute.attr);
	kobject_put(test_kobj);
}

module_init(sysfs_test_init);
module_exit(sysfs_test_exit);

MODULE_DESCRIPTION("sysfs example module.");
MODULE_LICENSE("GPL");

