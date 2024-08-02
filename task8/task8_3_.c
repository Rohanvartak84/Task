#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/string.h>
 
#define MAX_FILES 7
 
static struct kobject *kernel_object;
static int number;
static struct kobj_attribute *file_attrs[MAX_FILES]; // Array to keep track of attributes
 
static ssize_t file_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u32 rand_num;
    get_random_bytes(&rand_num, sizeof(rand_num));
    rand_num = rand_num % 100; // Random number between 0 and 99
    return sprintf(buf, "%u\n", rand_num);
}
 
static ssize_t number_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", number);
}
 
static ssize_t number_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int ret, value, i;
    char filename[20];
 
    // Convert string to integer
    ret = kstrtoint(buf, 10, &value);
 
    if (ret < 0 || value < 1 || value > 7)
        return -EINVAL;
 
    number = value;
 
    // Remove old files and attributes if any
    for (i = 0; i < MAX_FILES; i++) {
        if (file_attrs[i]) {
            sysfs_remove_file(kernel_object, &file_attrs[i]->attr);
            kfree(file_attrs[i]); // Free allocated attribute
            file_attrs[i] = NULL;
        }
    }
 
    // Create new files with random numbers
    for (i = 0; i < number; i++) {
        snprintf(filename, sizeof(filename), "file%d", i + 1);
 
        // Allocate and initialize attribute for each file
        file_attrs[i] = kmalloc(sizeof(*file_attrs[i]), GFP_KERNEL);
        if (!file_attrs[i])
            return -ENOMEM;
 
        sysfs_attr_init(&file_attrs[i]->attr);
        file_attrs[i]->attr.name = kstrdup(filename, GFP_KERNEL); // Use kstrdup to allocate memory for the name
        file_attrs[i]->attr.mode = 0444;
        file_attrs[i]->show = file_show;
        file_attrs[i]->store = NULL;
 
        if (sysfs_create_file(kernel_object, &file_attrs[i]->attr)) {
            kfree(file_attrs[i]->attr.name); // Free allocated name
            kfree(file_attrs[i]); // Free allocated attribute
            file_attrs[i] = NULL;
            return -ENOMEM;
        }
    }
 
    return count;
}
 
// Attribute for 'number'
static struct kobj_attribute number_attribute = {
    .attr = {
        .name = "number",
        .mode = 0666,
    },
    .show = number_show,
    .store = number_store,
};
 
static struct attribute *attrs[] = {
&number_attribute.attr,
    NULL,
};
 
static struct attribute_group attr_group = {
    .attrs = attrs,
};
 
static int __init sysfs_test_init(void)
{
    int retval;
 
    kernel_object = kobject_create_and_add("test", &THIS_MODULE->mkobj.kobj);
    if (!kernel_object)
        return -ENOMEM;
 
    retval = sysfs_create_group(kernel_object, &attr_group);
    if (retval) {
        kobject_put(kernel_object);
        return retval;
    }
 
    printk("sysfs module loaded\n");
    return 0;
}
 
static void __exit sysfs_test_exit(void)
{
    int i;
 
    // Remove created files and their attributes
    for (i = 0; i < MAX_FILES; i++) {
        if (file_attrs[i]) {
            sysfs_remove_file(kernel_object, &file_attrs[i]->attr);
            kfree(file_attrs[i]->attr.name); // Free allocated name
            kfree(file_attrs[i]); // Free allocated attribute
        }
    }
 
    // Remove the main kobject
    kobject_put(kernel_object);
    printk("sysfs module unloaded and deleted all the files\n");
}
 
module_init(sysfs_test_init);
module_exit(sysfs_test_exit);
 
MODULE_AUTHOR("YEPURI SIDDU");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sysfs test module to create files and store random numbers."); 
