#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/mydevf7"

void main(){
	int fd;
	int retval;
	char buffer[12];
	
	printf("opening file %s\n",DEVICE);

	fd = open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open file");
		return;
	}

	getchar();

	retval=write(fd,"hello world",sizeof("hello world"));
	printf("written %d bytes\n",retval);

	getchar();

	retval=read(fd,buffer,sizeof(buffer));
	printf("read %d bytes\n",retval);
	printf("user buffer: %s\n",buffer);
	
	getchar();

	printf("closing file\n");
	close(fd);
}
		


