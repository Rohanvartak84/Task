#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define GPIO_LED 68  // GPIO 68 corresponds to P8_10

static struct timer_list my_timer;
static int led_on = 0;

void my_timer_callback(struct timer_list *t) {
    led_on = !led_on;
    gpio_set_value(GPIO_LED, led_on);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000)); // 1 second interval
}

static int __init gpio_blink_init(void) {
    int ret;

    // Request GPIO
    ret = gpio_request(GPIO_LED, "sysfs");
    if (ret) {
        pr_err("gpio_request failed\n");
        return ret;
    }

    // Set GPIO direction
    ret = gpio_direction_output(GPIO_LED, led_on);
    if (ret) {
        pr_err("gpio_direction_output failed\n");
        gpio_free(GPIO_LED);
        return ret;
    }

    // Initialize and start the timer
    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    pr_info("GPIO Blink Driver Initialized\n");
    return 0;
}

static void __exit gpio_blink_exit(void) {
    del_timer(&my_timer);
    gpio_set_value(GPIO_LED, 0); // Turn off LED
    gpio_free(GPIO_LED);
    pr_info("GPIO Blink Driver Removed\n");
}

module_init(gpio_blink_init);
module_exit(gpio_blink_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple GPIO Blink Driver using GPIO 68 (P8_10)");

