#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/pseudo_char_device"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char write_buf[] = "Hello, Kernel!";
    char read_buf[BUFFER_SIZE];
    ssize_t ret;

    // Open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    // Write to the device
    ret = write(fd, write_buf, strlen(write_buf));
    if (ret < 0) {
        perror("Failed to write to the device");
        close(fd);
        return -1;
    }

    // Seek to the beginning of the device
    lseek(fd, 0, SEEK_SET);

    // Read from the device
    ret = read(fd, read_buf, BUFFER_SIZE);
    if (ret < 0) {
        perror("Failed to read from the device");
        close(fd);
        return -1;
    }

    // Null terminate the string read from the device
    read_buf[ret] = '\0';
    printf("Read from device: %s\n", read_buf);

    // Close the device
    close(fd);
    return 0;
}

