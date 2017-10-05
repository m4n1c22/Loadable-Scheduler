#MACROS
obj-m += process_queue.o
obj-m += process_scheduler.o
obj-m += process_set.o


PWD := $(shell pwd)
KVER := $(shell uname -r)
#Target option for Compiling all the tasks
default:
	make -C /lib/modules/$(KVER)/build SUBDIRS=$(PWD) modules
#Target option for inserting the generated kernel module into kernel.
insmod:
	sh insmod_scr.sh
#Target option for removing the generated kernel module from kernel.
rmmod:
	sh rmmod_scr.sh
#Target option for compiling and loading kernel module.
load: default insmod
#Target option for unloading and cleaning the generated kernel modules.
unload: rmmod clean


#Target option for cleaning the generated kernel modules.
clean:
	make -C /lib/modules/$(KVER)/build SUBDIRS=$(PWD) clean
