obj-m +=lab1_tasklet.o
obj-m +=lab1_workqueue_shared.o
obj-m +=lab1_interrupt.o
obj-m +=lab2_one_tasklet.o
obj-m +=lab2_one_workqueue.o
obj-m +=lab2_one_thread.o

KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
