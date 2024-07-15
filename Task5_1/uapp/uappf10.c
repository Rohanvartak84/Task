#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

#define DEVICE "/dev/mydevf10"

int main(){
	int fd;
	int retval;
	int value=10;

	printf("opening file: %s\n",DEVICE);
	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open failed");
		return -1;
	}
	
	retval = write(fd,&value,sizeof(value));
	printf("write retval: %d\n",retval);
	getchar();

	value=2;

	retval = read(fd,&value,sizeof(value));
	printf("read retval: %d\t value: %d\n",retval,value);
	getchar();

	printf("closing file\n");
	close(fd);
}
