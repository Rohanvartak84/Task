#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_COUNT 4
#define DEVICE_NAME "multi_char_device"
#define BUFFER_SIZE 1024

struct multi_device_data {
    char *buffer;
    struct cdev cdev;
};

static struct multi_device_data devices[DEVICE_COUNT];
static int major_number;
static struct class *my_class = NULL;

static int device_open(struct inode *inode, struct file *file) {
    struct multi_device_data *dev = container_of(inode->i_cdev, struct multi_device_data, cdev);
    file->private_data = dev; // Save the device data struct for later use
    printk(KERN_INFO "Device %d opened\n", MINOR(inode->i_rdev));
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device %d released\n", MINOR(inode->i_rdev));
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
    struct multi_device_data *dev = file->private_data;
    ssize_t bytes_read = 0;
    printk(KERN_INFO "Reading from device %d\n", MINOR(dev->cdev.dev));

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    bytes_read = count - copy_to_user(buf, dev->buffer + *offset, count);
    *offset += bytes_read;

    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    struct multi_device_data *dev = file->private_data;
    ssize_t bytes_written = 0;
    printk(KERN_INFO "Writing to device %d\n", MINOR(dev->cdev.dev));

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (*offset + count > BUFFER_SIZE)
        count = BUFFER_SIZE - *offset;

    if (count <= 0)
        return -EINVAL;

    if (!buf)
        return -EFAULT;

    bytes_written = count - copy_from_user(dev->buffer + *offset, buf, count);
    *offset += bytes_written;

    printk(KERN_INFO "Received data from user for device %d: %.*s\n", MINOR(dev->cdev.dev), (int)bytes_written, dev->buffer + *offset - bytes_written);

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

static int __init multi_char_devices_init(void) {
    int i, ret;
    dev_t dev;

    ret = alloc_chrdev_region(&dev, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0)
        return ret;

    major_number = MAJOR(dev);

    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev_region(dev, DEVICE_COUNT);
        return PTR_ERR(my_class);
    }

    for (i = 0; i < DEVICE_COUNT; ++i) {
        devices[i].buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
        if (!devices[i].buffer) {
            for (; i >= 0; --i)
                kfree(devices[i].buffer);
            class_destroy(my_class);
            unregister_chrdev_region(dev, DEVICE_COUNT);
            return -ENOMEM;
        }

        cdev_init(&devices[i].cdev, &fops);
        devices[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&devices[i].cdev, MKDEV(major_number, i), 1);
        if (ret < 0) {
            for (; i >= 0; --i) {
                cdev_del(&devices[i].cdev);
                kfree(devices[i].buffer);
            }
            class_destroy(my_class);
            unregister_chrdev_region(dev, DEVICE_COUNT);
            return ret;
        }

        device_create(my_class, NULL, MKDEV(major_number, i), NULL, "multi_char_device%d", i);
    }

    printk(KERN_INFO "Multi character devices registered\n");
    return 0;
}

static void __exit multi_char_devices_exit(void) {
    int i;
    dev_t dev = MKDEV(major_number, 0);

    for (i = 0; i < DEVICE_COUNT; ++i) {
        device_destroy(my_class, MKDEV(major_number, i));
        cdev_del(&devices[i].cdev);
        kfree(devices[i].buffer);
    }

    class_destroy(my_class);
    unregister_chrdev_region(dev, DEVICE_COUNT);

    printk(KERN_INFO "Multi character devices unregistered\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multi character driver");

module_init(multi_char_devices_init);
module_exit(multi_char_devices_exit);

