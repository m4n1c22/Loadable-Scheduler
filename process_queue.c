/**
	\file	:	process_queue.c
	\author	: 	Sreeram Sadasivam
	\brief	:	Process Queue Module dealing with the handling aspects of storage and
				retrieval of process information about a given process.
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
MODULE_DESCRIPTION("Process Queue Module");
MODULE_LICENSE("GPL");

/**Enumeration for Process States*/
enum process_state {
	
	eCreated		=	0, /**Process in Created State*/
	eRunning		=	1, /**Process in Running State*/
	eWaiting		=	2, /**Process in Waiting State*/
	eBlocked		=	3, /**Process in Blocked State*/
	eTerminated		=	4  /**Process in Terminate State*/
};

/** Structure for a process */
struct proc {

	int pid;
	enum process_state state;
	struct list_head list;
}top;


/**Function Prototypes for Process Queue Functions*/
int init_process_queue(void);
int release_process_queue(void);
int add_process_to_queue(int pid);
int remove_process_from_queue(int pid);
int print_process_queue(void);
int change_process_state_in_queue(int pid, int changeState);

/** Process Queue Functions */

/**
	Function Name : init_process_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for initializing a process queue.
*/
int init_process_queue(void) {

	printk(KERN_INFO "Initializing the Process Queue...\n");
	INIT_LIST_HEAD(&top.list);
	return 0;
}

/**
	Function Name : release_process_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for releasing a process queue.
*/
int release_process_queue(void) {
		 	
	struct proc *tmp, *node;
	printk(KERN_INFO "Releasing Process Queue...\n");
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		list_del(&node->list);
		kfree(node);
	}
	return 0;
}

/**
	Function Name : add_process_to_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for adding a process into a queue.
*/
int add_process_to_queue(int pid) {
			
	struct proc *new_process = kmalloc(sizeof(struct proc), GFP_KERNEL);
	
	new_process->pid = pid;
	
	new_process->state = eCreated;

	INIT_LIST_HEAD(&new_process->list);
	
	list_add_tail(&(new_process->list), &(top.list));
	
	return 0;
}

/**
	Function Name : remove_process_from_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for removing a given process 
					from the queue.
*/
int remove_process_from_queue(int pid) {
		 	
	struct proc *tmp, *node;
	
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		if(node->pid == pid) {
			printk(KERN_INFO "Removing the given Process %d from the  Process Queue...\n", pid);
			list_del(&node->list);
			kfree(node);
		}
	}
	return 0;
}

/**
	Function Name : change_process_state_in_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for changing the process state 
					for a given process in the queue.
*/
int change_process_state_in_queue(int pid, int changeState) {
		 	
	struct proc *tmp, *node;
	
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		if(node->pid == pid) {
			
			printk(KERN_INFO "Updating the process state the Process %d in  Process Queue...\n", pid);
			node->state = changeState;
		}
	}
	return 0;
}


/**
	Function Name : print_process_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for adding a process into a queue.
*/
int print_process_queue(void) {
			
	struct proc *tmp;
	printk(KERN_INFO "Process Queue: \n");
	list_for_each_entry(tmp, &(top.list), list) {
	
		printk(KERN_INFO "Process ID: %d\n", tmp->pid);
	}
	return 0;
}


/**
	Function Name : process_queue_module_init
	Function Type : Module INIT
	Description   : Initialization method of the Kernel module. The
			method gets invoked when the kernel module is being
			inserted using the command insmod.
*/
static int __init process_queue_module_init(void)
{
	printk(KERN_INFO "Process Queue module is being loaded.\n");
		
	/** Successful execution of initialization method. */
	return 0;
}

/**
	Function Name : process_queue_module_cleanup
	Function Type : Module EXIT
	Description   : Cleanup method of the Kernel module. The
                	method gets invoked when the kernel module is being
                 	removed using the command rmmod.
*/
static void __exit process_queue_module_cleanup(void)
{
	printk(KERN_INFO "Process Queue module is being unloaded.\n");
}
/** Initializing the kernel module init with custom init method */
module_init(process_queue_module_init);
/** Initializing the kernel module exit with custom cleanup method */
module_exit(process_queue_module_cleanup);

EXPORT_SYMBOL_GPL(init_process_queue);
EXPORT_SYMBOL_GPL(release_process_queue);
EXPORT_SYMBOL_GPL(add_process_to_queue);
EXPORT_SYMBOL_GPL(remove_process_from_queue);
EXPORT_SYMBOL_GPL(print_process_queue);
//EXPORT_SYMBOL_GPL(process_state);
EXPORT_SYMBOL_GPL(change_process_state_in_queue);