#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/gpio_blink"

int main() {
    int fd;

    // Open the device file
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }

    // Blink the LED in an infinite loop
    while (1) {
        // Toggle the LED by writing '1'
        if (write(fd, "1", 1) < 0) {
            perror("Failed to write to the device");
            close(fd);
            return EXIT_FAILURE;
        }

        // Wait for the blink interval
       sleep(1);
    }

    // Close the device (this line is never reached due to the infinite loop)
    close(fd);
    
    return EXIT_SUCCESS;
}

