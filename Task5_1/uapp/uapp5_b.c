#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEVICE "/dev/mydevf6"

void main(){

	int fd;

	printf("opening file :%s\n",DEVICE);

	sleep(2);

	fd=open(DEVICE,O_RDWR);
	if(fd<0){
		perror("fd");
		exit(1);
	}
	
	if(fork()==0){
		printf("child writing: %ld\n",write(fd,"hello world",sizeof("hello world")));
		exit(0);
	}
	else{
		printf("parent writing: %ld\n",write(fd,"bye world",sizeof("bye world")));
		wait(0);
		exit(0);
	}
	close(fd);
}




