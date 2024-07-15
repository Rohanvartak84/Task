#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/mydevf12"

int main(){
	int fd;
	int retval;
	char *buffer=malloc(sizeof(char)*10);

	printf("opening file: %s\n",DEVICE);
	fd=open(DEVICE,O_RDWR);

	if(fd<0){
		perror("open failed");
		exit(1);
	}
	getchar();

	strcpy(buffer,"hello");

	retval=write(fd,buffer,strlen(buffer));
	printf("write retval:%d\n",retval);
	getchar();

	printf("closing file\n");
	close(fd);
	return 0;
}
