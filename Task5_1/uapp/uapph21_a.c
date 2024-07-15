#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEVICE "/dev/mydevh21"

int main(){
	int fd;
	int retval;
	char buffer[20];
	int i;

	printf("opening file \n");
	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open failed");
		exit(1);
	}

	getchar();

	retval=write(fd,"hello",5);
	printf("write retval:%d\n",retval);
	getchar();
	
	retval=write(fd,"world",5);
	printf("write retval:%d\n",retval);
	getchar();

	printf("closing file\n");
	close(fd);
	return 0;
}
