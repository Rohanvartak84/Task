#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "pseudo_char_dev"
#define BUFFER_SIZE 1024

static int major_number;
static char *device_buffer;
static size_t buffer_size = BUFFER_SIZE;
static loff_t current_position = 0;

static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    ssize_t bytes_read;

    if (*offset >= buffer_size)
        return 0;

    if (*offset + count > buffer_size)
        count = buffer_size - *offset;

    bytes_read = count - copy_to_user(user_buffer, device_buffer + *offset, count);
    *offset += bytes_read;

    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    ssize_t bytes_written;

    if (*offset >= buffer_size)
        return 0;

    if (*offset + count > buffer_size)
        count = buffer_size - *offset;

    bytes_written = count - copy_from_user(device_buffer + *offset, user_buffer, count);
    *offset += bytes_written;

    return bytes_written;
}

static loff_t device_llseek(struct file *file, loff_t offset, int whence) {
    loff_t new_position;

    switch (whence) {
        case SEEK_SET:
            new_position = offset;
            break;
        case SEEK_CUR:
            new_position = current_position + offset;
            break;
        case SEEK_END:
            new_position = buffer_size + offset;
            break;
        default:
            return -EINVAL;
    }

    if (new_position < 0 || new_position > buffer_size)
        return -EINVAL;

    current_position = new_position;
    return current_position;
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
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major_number;
    }

    device_buffer = kmalloc(buffer_size, GFP_KERNEL);
    if (!device_buffer) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to allocate memory\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Device registered with major number %d\n", major_number);
    return 0;
}

static void __exit device_exit(void) {
    kfree(device_buffer);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple pseudo character device driver");
MODULE_VERSION("1.0");

