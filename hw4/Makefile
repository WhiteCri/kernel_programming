obj-m +=lab1_wait_event_many.o
obj-m +=lab1_wait_event_1.o
obj-m +=lab1_semaphore_additional.o
obj-m +=lab2_killable.o

KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
