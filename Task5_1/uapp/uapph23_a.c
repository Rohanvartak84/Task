#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	char buffer[50];
	int fd;
	int length;
	int i=0;

	fd=open("/dev/mydevh23",O_RDWR);
	if(fd<0){
		perror("open failed");
		exit(1);
	}
	printf("write: %ld\n",write(fd,"hello world",sizeof("hello world")));
	printf("write: %ld\n",write(fd,"bye world",sizeof("bye world")));
	
	lseek(fd,0,SEEK_SET);
	perror("lseek");
	memset(buffer,0,sizeof(buffer));

	length= read(fd,buffer,sizeof(buffer));
	buffer[length]= '\0';

	printf("read: %s\t length: %d\n",buffer,length);

	for(i=0;i<length;i++)
		printf("buffer[%d]:\t%c\n",i,buffer[i]);

	
	memset(buffer,0,sizeof(buffer));

        length= read(fd,buffer,sizeof(buffer));
        buffer[length]= '\0';

        printf("read: %s\t length: %d\n",buffer,length);
	close(fd);
}
