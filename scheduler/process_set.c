/**
	\file	:	process_set.c
	\author	: 	Sreeram Sadasivam
	\brief	:	Accessing a proc system file which set the process id
				which can be later used for our custom scheduler.
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
MODULE_DESCRIPTION("Process Setting Module");
MODULE_LICENSE("GPL");

/** PROC FS RELATED MACROS */
#define PROC_CONFIG_FILE_NAME	"process_sched_add"

/** STANDARD MACROS */
#define BASE_10 		10

/**Enumeration for Process States*/
enum process_state {
	
	eCreated		=	0, /**Process in Created State*/
	eRunning		=	1, /**Process in Running State*/
	eWaiting		=	2, /**Process in Waiting State*/
	eBlocked		=	3, /**Process in Blocked State*/
	eTerminated		=	4  /**Process in Terminate State*/
};

/**Enumeration for Function Execution*/
enum execution {

	eExecFailed 	= 	-1, /**Function executed failed.*/
	eExecSuccess 	=	 0  /**Function executed successfully.*/
};

/** Proc FS Dir Object */
static struct proc_dir_entry *proc_sched_add_file_entry;


/**External Function Prototypes for Process Queue Functions*/
extern int add_process_to_queue(int pid);
extern int remove_process_from_queue(int pid);
extern int print_process_queue(void);
extern int get_first_process_in_queue(void);
extern int remove_terminated_processes_from_queue(void);
extern int change_process_state_in_queue(int pid, int changeState);
/**
	Function Name : process_sched_add_module_read
	Function Type : Kernel Callback Method
	Description   : Method is invoked whenever the process_sched_add 
					file is	read. This callback method is triggered when
					a read operation performed on the above mentioned 
					file which is registered to the file operation 
					object.	
					Unfortunately, /proc/process_sched_add_module is a 
					write-only file
*/
static ssize_t process_sched_add_module_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	
	printk(KERN_INFO "Process Scheduler Add Module read.\n");
	//print_process_queue();
	printk(KERN_INFO "Next Executable PID in the list if RR Scheduling: %d\n", get_first_process_in_queue());
	/** Successful execution of read call back. EOF reached.*/
	return 0;
}


/**
	Function Name : process_sched_add_module_write
	Function Type : Kernel Callback Method
	Description   : Method is invoked whenever the process_sched_add
					file is written. This callback method is triggered 
					when a write operation performed on the above 
					mentioned file which is registered to the file 
					operation object. 
					/proc/process_sched_add is a write only file.
*/
static ssize_t process_sched_add_module_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int ret;
	long int new_proc_id;	
	
	printk(KERN_INFO "Process Scheduler Add Module write.\n");
	printk(KERN_INFO "Registered Process ID: %s\n", buf);
	
	ret = kstrtol(buf,BASE_10,&new_proc_id);
	if(ret < 0) {
		/** Invalid argument in conversion error.*/
		return -EINVAL;
	}
	
	/**	Add process to the process queue.*/
	ret = add_process_to_queue(new_proc_id);
	/**Check if the add process to queue method was successful or not.*/
	if(ret != eExecSuccess) {
		printk(KERN_ALERT "Process Set ERROR:add_process_to_queue function failed from sched set write method");
		/** Add process to queue error.*/
		return -ENOMEM;
	}

	/** Successful execution of write call back.*/
	return count;
}

/**
	Function Name : process_sched_add_module_open
	Function Type : Kernel Callback Method
	Description   : Method is invoked whenever the process_sched_add
					file is opened. This callback method is triggered 
					when an open operation performed on the above 
					mentioned file which is registered to the file 
					operation object. 
					/proc/process_sched_add is a write only file.
*/
static int process_sched_add_module_open(struct inode * inode, struct file * file)
{
	printk(KERN_INFO "Process Scheduler Add Module open.\n");
	
	/** Successful execution of open call back.*/
	return 0;
}

/**
	Function Name : process_sched_add_module_release
	Function Type : Kernel Callback Method
	Description   : Method is invoked whenever the process_sched_add
			file is closed. This callback method is triggered 
			when an close operation performed on the above 
			mentioned file which is registered to the file 
			operation object. 
			/proc/process_sched_add is a write only file.
*/
static int process_sched_add_module_release(struct inode * inode, struct file * file)
{
	printk(KERN_INFO "Process Scheduler Add Module released.\n");
	/** Successful execution of release callback.*/
	return 0;
}

/** File operations related to process_sched_add file */
static struct file_operations process_sched_add_module_fops = {
	.owner =	THIS_MODULE,
	.read =		process_sched_add_module_read,
	.write =	process_sched_add_module_write,
	.open =		process_sched_add_module_open,
	.release =	process_sched_add_module_release,
};

/**
	Function Name : process_sched_add_module_init
	Function Type : Module INIT
	Description   : Initialization method of the Kernel module. The
			method gets invoked when the kernel module is being
			inserted using the command insmod.
*/
static int __init process_sched_add_module_init(void)
{
	printk(KERN_INFO "Process Add to Scheduler module is being loaded.\n");
	
	/**Proc FS is created with RD&WR permissions with name process_sched_add*/
	proc_sched_add_file_entry = proc_create(PROC_CONFIG_FILE_NAME,0777,NULL,&process_sched_add_module_fops);
	/** Condition to verify if process_sched_add creation was successful*/
	if(proc_sched_add_file_entry == NULL) {
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",PROC_CONFIG_FILE_NAME);
		/** File Creation problem.*/
		return -ENOMEM;
	}
	
	
	/** Successful execution of initialization method. */
	return 0;
}

/**
	Function Name : process_sched_add_module_cleanup
	Function Type : Module EXIT
	Description   : Cleanup method of the Kernel module. The
                	method gets invoked when the kernel module is being
                 	removed using the command rmmod.
*/
static void __exit process_sched_add_module_cleanup(void)
{
	
	printk(KERN_INFO "Process Add to Scheduler module is being unloaded.\n");
	/** Proc FS object removed.*/
	proc_remove(proc_sched_add_file_entry);
}
/** Initializing the kernel module init with custom init method */
module_init(process_sched_add_module_init);
/** Initializing the kernel module exit with custom cleanup method */
module_exit(process_sched_add_module_cleanup);
