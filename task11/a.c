#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define GPIO_LED 68  // GPIO 68 corresponds to P8_10
#define GPIO2_BASE 0x481AC000 // Base address for GPIO2
#define GPIO_OE 0x134 // Offset for the Output Enable register
#define GPIO_DATAOUT 0x13C // Offset for the Data Out register

static struct timer_list my_timer;
static int led_on = 0;
static void __iomem *gpio2_base;
static unsigned int major_number;

// Timer callback to toggle the LED
void my_timer_callback(struct timer_list *t) {
    led_on = !led_on;
    if (led_on)
        iowrite32(ioread32(gpio2_base + GPIO_DATAOUT) | (1 << (GPIO_LED - 64)), gpio2_base + GPIO_DATAOUT);
    else
        iowrite32(ioread32(gpio2_base + GPIO_DATAOUT) & ~(1 << (GPIO_LED - 64)), gpio2_base + GPIO_DATAOUT);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000)); // 1 second interval
}

// Read LED status
static ssize_t gpio_blink_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int value = (ioread32(gpio2_base + GPIO_DATAOUT) >> (GPIO_LED - 64)) & 1;
    char buf[2];
    buf[0] = value ? '1' : '0';
    buf[1] = '\n';
    if (*offset != 0)
        return 0;
    if (copy_to_user(buffer, buf, 2))
        return -EFAULT;
    *offset = 2;
    return 2;
}

static struct file_operations fops = {
    .read = gpio_blink_read,
};

// Initialize the driver
static int __init gpio_blink_init(void) {
    int ret;

    // Request GPIO
    ret = gpio_request(GPIO_LED, "sysfs");
    if (ret) {
        pr_err("gpio_request failed\n");
        return ret;
    }

    // Map GPIO2 base address
    gpio2_base = ioremap(GPIO2_BASE, 0x1000);
    if (!gpio2_base) {
        pr_err("ioremap failed\n");
        gpio_free(GPIO_LED);
        return -ENOMEM;
    }

    // Set GPIO direction to output using MMIO
    iowrite32(ioread32(gpio2_base + GPIO_OE) & ~(1 << (GPIO_LED - 64)), gpio2_base + GPIO_OE);

    // Initialize and start the timer
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    // Register character device
    major_number = register_chrdev(0, "gpio_blink", &fops);
    pr_info("Major number:%d\n",major_number);
    if (major_number < 0) {
        pr_err("Failed to register a major number\n");
        gpio_free(GPIO_LED);
        iounmap(gpio2_base);
        return major_number;
    }

    pr_info("GPIO Blink Driver Initialized\n");
    return 0;
}

// Cleanup the driver
static void __exit gpio_blink_exit(void) {
    del_timer(&my_timer);
    iowrite32(ioread32(gpio2_base + GPIO_DATAOUT) & ~(1 << (GPIO_LED - 64)), gpio2_base + GPIO_DATAOUT);
    gpio_free(GPIO_LED);
    iounmap(gpio2_base);
    unregister_chrdev(major_number, "gpio_blink");
    pr_info("GPIO Blink Driver Removed\n");
}

module_init(gpio_blink_init);
module_exit(gpio_blink_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple GPIO Blink Driver using GPIO 68 (P8_10) with MMIO");

