#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc,char *argv[]){
	int fd;
	unsigned long long old_jiffies;
	unsigned long long new_jiffies;

	fd=open("/dev/mydevf9",O_RDWR);
	if(fd<0){
		perror("open failed");
		return -1;
	}

	if(read(fd,&old_jiffies,sizeof(old_jiffies)) != sizeof(old_jiffies)){
		printf("failed in reading first jiffies\n");
		exit(3);
	}
	else{
		printf("first read:%lld\n",old_jiffies);
	}
	sleep(1);
	if(read(fd,&new_jiffies,sizeof(new_jiffies)) != sizeof(new_jiffies)){
                printf("failed in reading second jiffies\n");
                exit(4);
        }
        else{
                printf("second read:%lld\n",new_jiffies);
        }

	printf("difference: %lld\n",(new_jiffies-old_jiffies));
	close(fd);
}
