#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	char buffer[1024];
int fd;
unsigned int length;
unsigned char ch = 'A';
int i=0;

fd = open("/dev/msg27",O_RDWR);
if(fd <0)
{
perror("fd is failed to  open");
exit(2);
}


ioctl(fd,0x01,&length);
printf("length %u\n",length);


ioctl(fd,0x03,&ch);
perror("ioctl");
lseek(fd,0,SEEK_SET);

length =read(fd, buffer , 1024);
perror("read");

printf("length %d\n",length);
buffer[1023] = '\0';

close(fd);
return 0;
}
