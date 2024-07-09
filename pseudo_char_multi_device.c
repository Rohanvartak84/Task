#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>

#include <linux/slab.h>

#define NUM_DEVICES 4
#define DEVICE_NAME_BASE "pseudo_char_dev"
#define BUFFER_SIZE 1024

static int major_number;
static char *device_buffers[NUM_DEVICES];
static struct cdev pseudo_char_cdev[NUM_DEVICES];
static struct class *pseudo_char_class;
static struct device *pseudo_char_device[NUM_DEVICES];

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Multiple Pseudo Character Devices Driver");
MODULE_VERSION("1.0");

static int device_open(struct inode *inode, struct file *file) {
    int minor = iminor(inode);

    if (minor < 0 || minor >= NUM_DEVICES)
        return -ENODEV;

    file->private_data = (void *)(uintptr_t)minor;
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    int minor = (uintptr_t)file->private_data;
    ssize_t bytes_read;

    if (minor < 0 || minor >= NUM_DEVICES)
        return -ENODEV;

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    bytes_read = count - copy_to_user(user_buffer, device_buffers[minor] + *offset, count);
    *offset += bytes_read;

    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    int minor = (uintptr_t)file->private_data;
    ssize_t bytes_written;

    if (minor < 0 || minor >= NUM_DEVICES)
        return -ENODEV;

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    bytes_written = count - copy_from_user(device_buffers[minor] + *offset, user_buffer, count);
    *offset += bytes_written;

    return bytes_written;
}

static loff_t device_llseek(struct file *file, loff_t offset, int whence) {
    int minor = (uintptr_t)file->private_data;
    loff_t new_position;

    if (minor < 0 || minor >= NUM_DEVICES)
        return -ENODEV;

    switch (whence) {
        case SEEK_SET:
            new_position = offset;
            break;
        case SEEK_CUR:
            new_position = file->f_pos + offset;
            break;
        case SEEK_END:
            new_position = BUFFER_SIZE + offset;
            break;
        default:
            return -EINVAL;
    }

    if (new_position < 0 || new_position > BUFFER_SIZE)
        return -EINVAL;

    file->f_pos = new_position;
    return new_position;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .llseek = device_llseek,
};


static int __init device_init(void) {
    int i, ret;
    dev_t dev_no;

    // Allocate a range of char device numbers
    ret = alloc_chrdev_region(&dev_no, 0, NUM_DEVICES, DEVICE_NAME_BASE);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to allocate major number\n");
        return ret;
    }

    major_number = MAJOR(dev_no);

    // Create device class
    pseudo_char_class = class_create(DEVICE_NAME_BASE);
    if (IS_ERR(pseudo_char_class)) {
        unregister_chrdev_region(MKDEV(major_number, 0), NUM_DEVICES);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(pseudo_char_class);
    }

    // Initialize and register each device
    for (i = 0; i < NUM_DEVICES; i++) {
        device_buffers[i] = kmalloc(BUFFER_SIZE, GFP_KERNEL);
        if (!device_buffers[i]) {
            printk(KERN_ALERT "Failed to allocate memory for device %d\n", i);
            for (int j = 0; j < i; j++) {
                device_destroy(pseudo_char_class, MKDEV(major_number, j));
                kfree(device_buffers[j]);
                cdev_del(&pseudo_char_cdev[j]);
            }
            class_destroy(pseudo_char_class);
            unregister_chrdev_region(MKDEV(major_number, 0), NUM_DEVICES);
            return -ENOMEM;
        }

        memset(device_buffers[i], 0, BUFFER_SIZE);

        cdev_init(&pseudo_char_cdev[i], &fops);
        pseudo_char_cdev[i].owner = THIS_MODULE;
        ret = cdev_add(&pseudo_char_cdev[i], MKDEV(major_number, i), 1);
        if (ret) {
            printk(KERN_ALERT "Failed to add cdev for device %d\n", i);
            kfree(device_buffers[i]);
            for (int j = 0; j < i; j++) {
                device_destroy(pseudo_char_class, MKDEV(major_number, j));
                kfree(device_buffers[j]);
                cdev_del(&pseudo_char_cdev[j]);
            }
            class_destroy(pseudo_char_class);
            unregister_chrdev_region(MKDEV(major_number, 0), NUM_DEVICES);
            return ret;
        }

        pseudo_char_device[i] = device_create(pseudo_char_class, NULL, MKDEV(major_number, i), NULL, DEVICE_NAME_BASE "%d", i);
        if (IS_ERR(pseudo_char_device[i])) {
            printk(KERN_ALERT "Failed to create device %d\n", i);
            cdev_del(&pseudo_char_cdev[i]);
            kfree(device_buffers[i]);
            for (int j = 0; j < i; j++) {
                device_destroy(pseudo_char_class, MKDEV(major_number, j));
                kfree(device_buffers[j]);
                cdev_del(&pseudo_char_cdev[j]);
            }
            class_destroy(pseudo_char_class);
            unregister_chrdev_region(MKDEV(major_number, 0), NUM_DEVICES);
            return PTR_ERR(pseudo_char_device[i]);
        }

        printk(KERN_INFO "Device %d registered with major number %d\n", i, major_number);
    }

    return 0;
}

static void __exit device_exit(void) {
    int i;

    for (i = 0; i < NUM_DEVICES; i++) {
        device_destroy(pseudo_char_class, MKDEV(major_number, i));
        kfree(device_buffers[i]);
        cdev_del(&pseudo_char_cdev[i]);
        printk(KERN_INFO "Device %d unregistered\n", i);
    }

    class_destroy(pseudo_char_class);
    unregister_chrdev_region(MKDEV(major_number, 0), NUM_DEVICES);
}

module_init(device_init);
module_exit(device_exit);

