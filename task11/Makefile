obj-m := b.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR= /home/rohan842/rohan/linux_bbb/linux/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean

