#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
int fd;

fd = open("/dev/msg26",O_RDWR);


perror("fd");

ioctl(fd,0x01,10);

perror("ioctl");

getchar();

ioctl(fd,0x01);
perror("ioctl");

getchar();
close(fd);
return 0;
}
