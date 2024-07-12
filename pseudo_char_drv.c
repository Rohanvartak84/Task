#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "pseudo_char_device"
#define BUFFER_SIZE 1024

static char *device_buffer;
static int major_number;
static struct cdev my_cdev;
static struct class *my_class = NULL;
static struct device *my_device = NULL;

static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device released\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
    ssize_t bytes_read = 0;

    printk(KERN_INFO "Reading from device\n");

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    bytes_read = count - copy_to_user(buf, device_buffer + *offset, count);
    *offset += bytes_read;

    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    ssize_t bytes_written = 0;

    printk(KERN_INFO "Writing to device\n");

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    bytes_written = count - copy_from_user(device_buffer + *offset, buf, count);
    *offset += bytes_written;

printk(KERN_INFO "Received data from user: %.*s\n", (int)bytes_written, device_buffer + *offset - bytes_written);

    return bytes_written;
}

static loff_t device_llseek(struct file *file, loff_t offset, int whence) {
    loff_t new_pos;

    switch (whence) {
        case 0: // SEEK_SET
            new_pos = offset;
            break;
        case 1: // SEEK_CUR
            new_pos = file->f_pos + offset;
            break;
        case 2: // SEEK_END
            new_pos = BUFFER_SIZE + offset;
            break;
        default:
            return -EINVAL;
    }

    if (new_pos < 0 || new_pos > BUFFER_SIZE)
        return -EINVAL;

    file->f_pos = new_pos;
    return new_pos;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .llseek = device_llseek
};

static int __init pseudo_device_init(void) {
    int ret;
    dev_t dev;

    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer)
        return -ENOMEM;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        kfree(device_buffer);
        return ret;
    }

    major_number = MAJOR(dev);
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        kfree(device_buffer);
        return ret;
    }

    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        kfree(device_buffer);
        return PTR_ERR(my_class);
    }

    my_device = device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        kfree(device_buffer);
        return PTR_ERR(my_device);
    }

    printk(KERN_INFO "Pseudo character device registered\n");
    return 0;
}

static void __exit pseudo_device_exit(void) {
    dev_t dev = MKDEV(major_number, 0);

    device_destroy(my_class, dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    kfree(device_buffer);

    printk(KERN_INFO "Pseudo character device unregistered\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rohan");
MODULE_DESCRIPTION("A pseudo character driver");

module_init(pseudo_device_init);
module_exit(pseudo_device_exit);

