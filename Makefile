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
unload: rmmod clean_modules

#Target option for compiling the test_process program.
comp_test:
	gcc test_pr.c -o test_pr.out
#Target option for running the test_process program.
test: comp_test 
	./test_pr.out

#Target option for cleaning the generated kernel modules.
clean_modules:
	make -C /lib/modules/$(KVER)/build SUBDIRS=$(PWD) clean
#Target option for cleaning the test_process program.
clean_test:
	rm -f ./test_pr.out
#Target option for cleaning the test_process program and the generated kernel modules
cleanall: clean_test clean_modules
