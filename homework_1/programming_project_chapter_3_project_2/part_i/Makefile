obj-m+=listing_all_current_tasks.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C/lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
