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
	char buffer[10];
	int i;

	printf("opening file: %s\n",DEVICE);
	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open failed");
		exit(1);
	}

	getchar();


	for(i=0;i<250;i++){
		retval=write(fd,"hello",5);
		printf("write retval:%d\n",retval);
		if(retval !=5)
			break;
	}
	printf("closing file\n");
	close(fd);
	return 0;
}
