#obj-m +=test1.o
#obj-m +=test2.o
#obj-m +=test3.o
#obj-m +=test4.o
#obj-m +=test5.o
#obj-m +=test5_a.o
#obj-m +=test5_b.o
#obj-m +=fops6.o
#obj-m +=fops7.o
#obj-m +=fops8.o
#obj-m +=fops_9.o
#obj-m +=fops10.o
#obj-m +=fops_10.o
#obj-m +=fops12.o
#obj-m +=fops3.o
#obj-m +=fops4.o
#obj-m +=fops5.o
#obj-m +=fops10.o
#obj-m +=test11.o
#obj-m +=hello20.o
#obj-m +=hello21.o
#obj-m +=hello22.o
#obj-m +=hello23.o
#obj-m +=hello24.o
#obj-m +=hello25.o
#obj-m +=hello26.o
#obj-m +=hello27.o
obj-m +=hello28.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean



