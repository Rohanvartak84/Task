#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include </usr/src/linux-6.8.2/char_device_driver/kernel_prog/mystruct.h>


#define DEVICE "/dev/mydevf11"

void main(){
	int fd;
	int retval;
	int k_val,u_val=12345;
	abc user;

	user.i=10;
	user.c='r';

	printf("opening file %s\n",DEVICE);

	fd = open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open file");
		return;
	}

	getchar();

	retval=write(fd,&user,sizeof(user));
	printf("written %d bytes\n",retval);

	getchar();

	retval=read(fd,&user,sizeof(user));
	printf("read %d bytes\n",retval);
	printf("user value int:%d\tchar:%c\n",user.i,user.c);
	
	getchar();

	printf("closing file\n");
	close(fd);
}
		


