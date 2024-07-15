#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	char buffer[50];
	int fd1,fd2;
	int length;
	int i=0;

	fd1=open("/dev/mydevh250",O_RDWR);
	if(fd1<0){
		perror("open failed");
		exit(1);
	}
	fd2=open("/dev/mydevh251",O_RDWR);
        if(fd2<0){
                perror("open failed");
                exit(1);
        }
	printf("write: %ld\n",write(fd1,"hello world",sizeof("hello world")));
	lseek(fd2,0,SEEK_SET);
	perror("lseek");
	memset(buffer,0,sizeof(buffer));
	length= read(fd2,buffer,sizeof(buffer));
	buffer[length]= '\0';
	printf("read: %s\t length: %d\n",buffer,length);
	close(fd1);
	close(fd2);
}
