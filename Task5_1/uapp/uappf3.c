#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/mydevf3"

void main(){
	int fd;
	int retval;
	char buffer[20];

	printf("opening file: %s\n",DEVICE);

	fd = open(DEVICE,O_RDWR);
	if(fd<0){
		perror("open file");
		return;
	}
	close(fd);

	getchar();
	printf("opening file: %s\n",DEVICE);
	fd=open(DEVICE,O_WRONLY);
        if(fd<0)
        {
                perror("open fail");
                return;
        }
        close(fd);

	getchar();
	printf("opening file: %s\n",DEVICE);
        fd=open(DEVICE,O_RDWR);
        if(fd<0)
        {
                perror("open fail");
                return;
        }
        close(fd);

}

		


