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
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_AUTHOR("Sreeram Sadasivam");
MODULE_DESCRIPTION("Process Scheduler Module");
MODULE_LICENSE("GPL");

/**Macros*/
#define ALL_REG_PIDS	-100

/**Enumeration for Process States*/
enum process_state {
	
	eCreated		=	0, /**Process in Created State*/
	eRunning		=	1, /**Process in Running State*/
	eWaiting		=	2, /**Process in Waiting State*/
	eBlocked		=	3, /**Process in Blocked State*/
	eTerminated		=	4  /**Process in Terminate State*/
};



/**External Function Prototypes for Process Queue Functions*/
extern int add_process_to_queue(int pid);
extern int remove_process_from_queue(int pid);
extern int print_process_queue(void);
extern int change_process_state_in_queue(int pid, int changeState);
extern int get_first_process_in_queue(void);
extern int remove_terminated_processes_from_queue(void);

/**Function Prototype for Scheduler*/
static void context_switch(void);
int static_round_robin_scheduling(void);

/**Flags*/
static int flag = 0;

/**Time Quantum storage variable for pre-emptive based schedulers.*/
static int time_quantum;

/**Current PID*/
static int current_pid = -1;

/** WorkQueue Object */
struct workqueue_struct *scheduler_wq;


/** Creating a delayed_work object with the provided function handler.*/
static DECLARE_DELAYED_WORK(scheduler_hdlr, context_switch);
/**
	Function Name : context_switch
	Function Type : Internal Method
	Description   : Method which is invoked to switch the currently executing
					process with another process. The method internally calls
					the provided scheduling policy.
*/
static void context_switch(void){
	
	/** Boolean status of the queue.*/
	bool q_status=false;
	
	printk(KERN_ALERT "Scheduler instance: Context Switch\n");

	/**Invoking the static round robin scheduling policy.*/
	static_round_robin_scheduling();

	/** Condition check for producer unloading flag set or not.*/
	if (flag == 0){
		/** Setting the delayed work execution for the provided rate */
		q_status = queue_delayed_work(scheduler_wq, &scheduler_hdlr, time_quantum*HZ);
	}
	else
		printk(KERN_ALERT "Scheduler instance: scheduler is unloading\n");
}

/**
	Function Name : static_round_robin_scheduling
	Function Type : Scheduling Scheme
	Description   : Method for static round robin scheduling scheme.
*/
int static_round_robin_scheduling(void)
{
	/**Storage class variable to detecting the process state change.*/
	int ret_process_state=-1;

	printk(KERN_INFO "Static Round Robin Scheduling scheme.\n");
	
	remove_terminated_processes_from_queue();

	/**Check if the current process id is INVALID or not.*/
	if(current_pid != -1) {
		/**Add the current process to the process queue.*/	
		add_process_to_queue(current_pid);	
	}

	/** Obtaining the first process in the wait queue.*/
	current_pid = get_first_process_in_queue();
	/**
		Check if the obtained process id is invalid or not. If Invalid indicates,
		the queue does not contain any active process.
	*/
	if(current_pid != -1) {
		/**Change the process state of the obtained process from queue to running.*/
		ret_process_state = change_process_state_in_queue(current_pid, eRunning);
		/**Remove the process from the waiting queue.*/
		remove_process_from_queue(current_pid);
	}
	
	printk(KERN_INFO "Currently running process: %d\n", current_pid);

	/**Check if there no processes active in the scheduler or not.*/
	if(current_pid != -1) {
		printk(KERN_INFO "Current Process Queue...\n");
		/**Print the wait queue.*/
		print_process_queue();
		printk(KERN_INFO "Currently running process: %d\n", current_pid);
	}
	
	
	/** Successful execution of the method. */
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
	/** Boolean status of the queue.*/
	bool q_status=false;

	/**Initializing the time_quantum*/
	time_quantum = 3;

	printk(KERN_INFO "Process Scheduler module is being loaded.\n");
	
	/**
		Allocating the workqueue under the name scheduler-wq and max 1
		active schedulers.
	*/
	scheduler_wq = alloc_workqueue("scheduler-wq", WQ_UNBOUND, 1);

	/** Condition check if the workqueue allocation failed */
	if (scheduler_wq== NULL){
		
		printk(KERN_ERR "Scheduler instance ERROR:Workqueue cannot be allocated\n");
		/** Memory Allocation Problem */
		return -ENOMEM;
	}
	else {
		/** Performing an internal call for context_switch */
		context_switch();
		/** Setting the delayed work execution for the provided rate */
		q_status = queue_delayed_work(scheduler_wq, &scheduler_hdlr, time_quantum*HZ);
	}


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
	/** Signalling the scheduler module unloading */
	flag = 1;
	/** Cancelling pending jobs in the Work Queue.*/
	cancel_delayed_work(&scheduler_hdlr);
	/** Removing all the pending jobs from the Work Queue*/
	flush_workqueue(scheduler_wq);
	/** Deallocating the Work Queue */
	destroy_workqueue(scheduler_wq);

	printk(KERN_INFO "Process Scheduler module is being unloaded.\n");
}
/** Initializing the kernel module init with custom init method */
module_init(process_scheduler_module_init);
/** Initializing the kernel module exit with custom cleanup method */
module_exit(process_scheduler_module_cleanup);