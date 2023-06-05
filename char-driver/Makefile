obj-m = hello.o

#KERNEL_DIR := (uncomment and add path to your kerneldir)


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	#make -C $(KERNEL_DIR) M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	#make -C $(KERNEL_DIR) M=$(PWD) clean
