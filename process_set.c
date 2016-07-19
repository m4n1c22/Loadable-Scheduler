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

MODULE_AUTHOR("Sreeram Sadasivam");
MODULE_DESCRIPTION("Process Setting Module");
MODULE_LICENSE("GPL");

/** PROC FS RELATED MACROS */
#define PROC_CONFIG_FILE_NAME	"process_sched_add"


/** Proc FS Dir Object */
static struct proc_dir_entry *proc_sched_add_file_entry;


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
	printk(KERN_INFO "Process Scheduler Add Module write.\n");
	
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
	Function Name : gen_module_init
	Function Type : Module INIT
	Description   : Initialization method of the Kernel module. The
			method gets invoked when the kernel module is being
			inserted using the command insmod.
*/
static int __init gen_module_init(void)
{
	printk(KERN_INFO "Process Scheduler module is being loaded.\n");
	
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
	Function Name : gen_module_cleanup
	Function Type : Module EXIT
	Description   : Cleanup method of the Kernel module. The
                	method gets invoked when the kernel module is being
                 	removed using the command rmmod.
*/
static void __exit gen_module_cleanup(void)
{
	printk(KERN_INFO "Process Scheduler module is being unloaded.\n");
	/** Proc FS object removed.*/
	proc_remove(proc_sched_add_file_entry);
}
/** Initializing the kernel module init with custom init method */
module_init(gen_module_init);
/** Initializing the kernel module exit with custom cleanup method */
module_exit(gen_module_cleanup);
