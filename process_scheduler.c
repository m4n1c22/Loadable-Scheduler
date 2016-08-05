/**
	\file	:	process_scheduler.c
	\author	: 	Sreeram Sadasivam
	\brief	:	Process Scheduler Module dealing with execution of custom scheduler.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR("Sreeram Sadasivam");
MODULE_DESCRIPTION("Process Scheduler Module");
MODULE_LICENSE("GPL");


/**Function Prototypes for Process Queue Functions*/
extern int init_process_queue(void);
extern int release_process_queue(void);
extern int add_process_to_queue(int pid);
extern int remove_process_from_queue(int pid);
extern int print_process_queue(void);
extern int change_process_state_in_queue(int pid, int changeState);


/**
	Function Name : round_robin_scheduling_init
	Function Type : Scheduling Initialization
	Description   : Initialization method for rounnd robin scheduling scheme.
*/
int round_robin_scheduling_init(void)
{
	printk(KERN_INFO "Round Robin Scheduling scheme initialized.\n");
		
	/** Successful execution of initialization method. */
	return 0;
}

/**
	Function Name : SJF_scheduling_init
	Function Type : Scheduling Initialization
	Description   : Initialization method for Shortest Job First scheduling scheme.
*/
int SJF_scheduling_init(void)
{
	printk(KERN_INFO "Shortest Job Scheduling scheme initialized.\n");
		
	/** Successful execution of initialization method. */
	return 0;
}


/**
	Function Name : process_scheduler_module_init
	Function Type : Module INIT
	Description   : Initialization method of the Kernel module. The
			method gets invoked when the kernel module is being
			inserted using the command insmod.
*/
static int __init process_scheduler_module_init(void)
{
	printk(KERN_INFO "Process Scheduler module is being loaded.\n");
		
	/** Successful execution of initialization method. */
	return 0;
}

/**
	Function Name : process_scheduler_module_cleanup
	Function Type : Module EXIT
	Description   : Cleanup method of the Kernel module. The
                	method gets invoked when the kernel module is being
                 	removed using the command rmmod.
*/
static void __exit process_scheduler_module_cleanup(void)
{
	printk(KERN_INFO "Process Scheduler module is being unloaded.\n");
}
/** Initializing the kernel module init with custom init method */
module_init(process_scheduler_module_init);
/** Initializing the kernel module exit with custom cleanup method */
module_exit(process_scheduler_module_cleanup);