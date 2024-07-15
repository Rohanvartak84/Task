#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/mydevf9"

void main(){
	int fd;
	int retval;
	int k_val,u_val=12345;
	
	printf("opening file %s\n",DEVICE);

	fd = open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open file");
		return;
	}

	getchar();

	retval=write(fd,&u_val,sizeof(int));
	printf("written %d bytes\n",retval);

	getchar();

	retval=read(fd,&k_val,sizeof(int));
	printf("read %d bytes\n",retval);
	printf("user value: %d\n",k_val);
	
	getchar();

	printf("closing file\n");
	close(fd);
}
		


