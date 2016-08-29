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
#include <linux/sched.h>
MODULE_AUTHOR("Sreeram Sadasivam");
MODULE_DESCRIPTION("Process Queue Module");
MODULE_LICENSE("GPL");

/**Macros*/
#define ALL_REG_PIDS	-100
#define	INVALID_PID		-1

/**Enumeration for Process States*/
enum process_state {
	
	eCreated		=	0, /**Process in Created State*/
	eRunning		=	1, /**Process in Running State*/
	eWaiting		=	2, /**Process in Waiting State*/
	eBlocked		=	3, /**Process in Blocked State*/
	eTerminated		=	4  /**Process in Terminate State*/
};


/**Enumeration for Task Errors*/
enum task_status_code {

	eTaskStatusExist 		= 	0,	/**Task is still active in the OS.*/
	eTaskStatusTerminated 	=  -1   /**Task has terminated.1*/
};

/** Structure for a process */
struct proc {

	int pid; 					/**Process ID*/
	enum process_state state;	/**Process State*/
	struct list_head list;		/**List pointer for generating a list of processes.*/
	/**More things to come in future such as nice value, priority etc,.*/
}top;

/**Semaphore for process queue*/
static struct semaphore mutex;

/**Function Prototypes for Task Queue Functions*/
enum task_status_code task_status_change(int pid, enum process_state eState);
enum task_status_code is_task_exists(int pid);

/**Function Prototypes for Process Queue Functions*/
int init_process_queue(void);
int release_process_queue(void);
int add_process_to_queue(int pid);
int remove_process_from_queue(int pid);
int print_process_queue(void);
int change_process_state_in_queue(int pid, int changeState);
int get_first_process_in_queue(void);
int remove_terminated_processes_from_queue(void);

/** Process Queue Functions */

/**
	Function Name : init_process_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for initializing a process queue.
*/
int init_process_queue(void) {

	printk(KERN_INFO "Initializing the Process Queue...\n");
	/**Generating the head of the queue and initializing an empty process queue.*/
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
	/**
		Iterating over the list of nodes pertaining to the process information
		and removing one by one.
	*/
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		/**Deleting link pointer established by the node to the list.*/
		list_del(&node->list);
		/**Removing the whole node.*/
		kfree(node);
	}
	/**Function returns success.*/
	return 0;
}

/**
	Function Name : add_process_to_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for adding a process into a queue.
*/
int add_process_to_queue(int pid) {
			
	/**Allocating space for the newly registered process.*/
	struct proc *new_process = kmalloc(sizeof(struct proc), GFP_KERNEL);
	
	/**Check if the kmalloc call was successful or not.*/	
	if(!new_process) {

		printk(KERN_ALERT "Process Queue ERROR:kmalloc function failed from add_process_to_queue function.");
		/** Add process to queue error.*/
		return -ENOMEM;
	}
	/**Setting the process id to the process info node new_process*/
	new_process->pid = pid;
	/**Setting the process state to the process info node new_process as waiting.*/
	new_process->state = eWaiting;
	/**Make the task level alteration therefore the process pauses its execution since in wait state.*/
	task_status_change(new_process->pid, new_process-> state);//TODO:Error handling to be added.

	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from add function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}

	/**Initialize the new process list as the new head.*/
	INIT_LIST_HEAD(&new_process->list);
	/**Set the new process as a tail to the previous top of the list.*/
	list_add_tail(&(new_process->list), &(top.list));
	
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);

	printk(KERN_INFO "Adding the given Process %d to the  Process Queue...\n", pid);
	/**Function executed successfully.*/
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
	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from remove function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}
	/**Iterating over the process queue and removing the process with provided pid.*/	
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		/**Check if the node pid is the same as the required pid.*/
		if(node->pid == pid) {
			printk(KERN_INFO "Removing the given Process %d from the  Process Queue...\n", pid);
			/**Deleting link pointer established by the node to the list.*/
			list_del(&node->list);
			/**Removing the whole node.*/
			kfree(node);
		}
	}
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);
	/**Function executed successfully.*/
	return 0;
}

/**
	Function Name : remove_terminated_processes_from_queue
	Function Type : Queue Job
	Description	  :	Method is invoked for removing all terminated processes
					from the queue.
*/
int remove_terminated_processes_from_queue(void) {
		 	
	struct proc *tmp, *node;
	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from remove function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}	
	/**Iterate over the process queue and remove all terminated processes from the queue.*/
	list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
		/**Check if the process is terminated or not.*/
		if(node->state == eTerminated) {
			printk(KERN_INFO "Removing the terminated Process %d from the  Process Queue...\n", node->pid);
			/**Deleting link pointer established by the node to the list.*/
			list_del(&node->list);
			/**Removing the whole node.*/
			kfree(node);
		}
	}
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);
	/**Function executed successfully.*/
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

	/**Enumeration to expect the task_status change function call.*/
	enum process_state ret_process_change_status;

	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from change process state function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}	
	/**Check if all registered PIDs are modified for state*/
	if(pid == ALL_REG_PIDS) {
		/**Iterate over all the processes in the queue and set the status the provided status.*/
		list_for_each_entry_safe(node, tmp, &(top.list), list) {
	
			printk(KERN_INFO "Updating the process state the Process %d in  Process Queue...\n", node->pid);
			/**Update the state to the provided state.*/
			node->state = changeState;
			/**Check if the task associated with the iterated node still exists or not.*/
			if(task_status_change(node->pid, node->state)==eTaskStatusTerminated) {
				/**Change state to terminated. Later handled by calling removeallterminated processes method*/	
				node->state = eTerminated;
			}
		}
	}
	else {
		/**Iterate over the queue and update the provided process with the state change.*/
		list_for_each_entry_safe(node, tmp, &(top.list), list) {
		
			/**Check if the iterated node is the required process or not.*/
			if(node->pid == pid) {
				
				printk(KERN_INFO "Updating the process state the Process %d in  Process Queue...\n", pid);
				/**Update the state to the provided state.*/
				node->state = changeState;
				/**Check if the task associated with the iterated node still exists or not.*/
				if(task_status_change(node->pid, node->state)==eTaskStatusTerminated) {
					/**Change state to terminated. Later handled by calling removeallterminated processes method*/	
					node->state = eTerminated;
					/**Return value updated to notify that the requested process is already terminated.*/
					ret_process_change_status = eTerminated;
				}
				//TODO:Else condition to be added.				
			}
			else {
				/**Check if the task associated with the iterated node exists or not.*/
				if(is_task_exists(node->pid)==eTaskStatusTerminated) {
					/**Change state to terminated. Later handled by calling removeallterminated processes method*/
					node->state = eTerminated;
				}
			}
		}
	}
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);

	/**Return the process status change associated with the internal call to task status change method.*/
	return ret_process_change_status;
}


/**
	Function Name : print_process_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for printing the process queue.
*/
int print_process_queue(void) {
			
	struct proc *tmp;
	printk(KERN_INFO "Process Queue: \n");
	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from print function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}	
	/**Iterate over the queue and print each process id.*/
	list_for_each_entry(tmp, &(top.list), list) {
	
		printk(KERN_INFO "Process ID: %d\n", tmp->pid);
	}
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);
	/**Function executed successfully.*/
	return 0;
}

/**
	Function Name : get_first_process_in_queue
	Function Type : Queue Function
	Description	  :	Method is invoked for getting the first process in the queue.
*/
int get_first_process_in_queue(void) {

	struct proc *tmp;
	/**Initially set the process id value as an INVALID value.*/
	int pid = INVALID_PID;
	/** 
		Condition to verify the down operation on the binary semaphore
		mutex. Entry into a Mutually exclusive block is granted by
		having a successful lock with the mentioned semaphore.
		mutex semaphore provides a safe access to the following
		critical section.
	*/
	if(down_interruptible(&mutex)){
		printk(KERN_ALERT "Process Queue ERROR:Mutual Exclusive position access failed from print function");
		/** Issue a restart of syscall which was supposed to be executed.*/
		return -ERESTARTSYS;
	}	

	/**Iterate over the process queue and find the first active process.*/
	list_for_each_entry(tmp, &(top.list), list) {
		/**Check if the task associated with the process is terminated or not.*/
		if((pid==INVALID_PID)&&(is_task_exists(tmp->pid)==eTaskStatusExist)) {
			/**Set the process id to read process.*/
			pid = tmp->pid;	
		}
		/*else if(is_task_exists(tmp->pid)==eTaskStatusTerminated) {
			tmp->state = eTerminated;
		}*/
	}
	/** 
		Performing an up operation on mutex. Such an operation
		indicates the critical section is released for other
		processes/threads.
	*/
	up(&mutex);

	/**Returns the first process ID*/
	return pid;
}

/**
	Function Name : is_task_exists
	Function Type : Task level Existence
	Description   : Method checks if the task exists.
*/
enum task_status_code is_task_exists(int pid) {
	
	/**Task structure construct.*/
	struct task_struct *current_pr;	
	/**Obtain the task struct associated with provided PID.*/
	current_pr = pid_task(find_vpid(pid), PIDTYPE_PID);
	/**Check if the task exists or not by checking for NULL Value.*/
	if(current_pr == NULL) {
		/**Return the task status code as terminated.*/
		return eTaskStatusTerminated;
	}
	/**Return the task status code as exists.*/
	return eTaskStatusExist;
}


/**
	Function Name : task_status_change
	Function Type : Task level State change.
	Description   : Method changes the status of the task.
*/
enum task_status_code task_status_change(int pid, enum process_state eState) {

	/**Task structure construct.*/
	struct task_struct *current_pr;
	/**Obtain the task struct associated with provided PID.*/
	current_pr = pid_task(find_vpid(pid), PIDTYPE_PID);
	/**Check if the task exists or not by checking for NULL Value.*/
	if(current_pr == NULL) {
		/**Return the task status code as terminated.*/
		return eTaskStatusTerminated;
	}
	/**Check if the state change was Running.*/
	if(eState == eRunning) {

		/**Trigger a signal to continue the given task associated with the process.*/
		kill_pid(task_pid(current_pr), SIGCONT, 1);
		printk(KERN_INFO "Task status change to Running\n");
	}
	/**Check if the state change was Waiting.*/
	else if(eState == eWaiting) {
		/**Trigger a signal to pause the given task associated with the process.*/
		kill_pid(task_pid(current_pr), SIGSTOP, 1);
		printk(KERN_INFO "Task status change to Waiting\n");
	}
	/**Check if the state change was Blocked.*/
	else if(eState == eBlocked) {

		printk(KERN_INFO "Task status change to Blocked\n");
	}
	/**Check if the state change was Terminated.*/
	else if(eState == eTerminated) {

		printk(KERN_INFO "Task status change to Terminated\n");
	}
	/**Return the task status code as exists.*/
	return eTaskStatusExist;
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
	/** Initializing the semaphores */
	/** 
		Setting Mutex used for critical section inside fifo modules
		as 1. Indicates the critical section is free from use.
	*/
	sema_init(&mutex,1); 		
	
	/**Initializing the process queue*/
	init_process_queue();

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
	/**Releasing the process queue.*/
	release_process_queue();
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
EXPORT_SYMBOL_GPL(get_first_process_in_queue);
EXPORT_SYMBOL_GPL(change_process_state_in_queue);
EXPORT_SYMBOL_GPL(remove_terminated_processes_from_queue);