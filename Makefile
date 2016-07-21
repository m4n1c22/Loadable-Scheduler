#MACROS
obj-m += process_queue.o
obj-m += process_scheduler.o
obj-m += process_set.o
PWD := $(shell pwd)
KVER := $(shell uname -r)
#Target option for Compiling all the tasks
default:
	make -C /lib/modules/$(KVER)/build SUBDIRS=$(PWD) modules
clean:
	make -C /lib/modules/$(KVER)/build SUBDIRS=$(PWD) clean
