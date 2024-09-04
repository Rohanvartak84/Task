#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define GPIO_LED 68  // GPIO 68 corresponds to P8_10
#define GPIO2_BASE 0x481AC000 // Base address for GPIO2
#define GPIO_OE 0x134 // Offset for the Output Enable register
#define GPIO_DATAOUT 0x13C // Offset for the Data Out register

static void __iomem *gpio2_base;
static unsigned int major_number;
static int led_on = 0;

// Function to toggle LED state
static void toggle_led(void) {
    led_on = !led_on;
    if (led_on)
        iowrite32(ioread32(gpio2_base + GPIO_DATAOUT) | (1 << (GPIO_LED - 64)), gpio2_base + GPIO_DATAOUT);
    else
        iowrite32(ioread32(gpio2_base + GPIO_DATAOUT) & ~(1 << (GPIO_LED - 64)), gpio2_base + GPIO_DATAOUT);
}

// Write function to control LED from user space
static ssize_t gpio_blink_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char command;

    if (copy_from_user(&command, buffer, 1))
        return -EFAULT;

    if (command == '1') {
        toggle_led();  // Toggle LED when '1' is written
    }

    return len;
}

static struct file_operations fops = {
    .write = gpio_blink_write,
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

    // Register character device
    major_number = register_chrdev(0, "gpio_blink", &fops);
    pr_info("major number:%d\n",major_number);
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
    gpio_set_value(GPIO_LED, 0); // Turn off LED
    gpio_free(GPIO_LED);
    iounmap(gpio2_base);
    unregister_chrdev(major_number, "gpio_blink");
    pr_info("GPIO Blink Driver Removed\n");
}

module_init(gpio_blink_init);
module_exit(gpio_blink_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A GPIO LED Control Driver using GPIO 68 (P8_10)");

