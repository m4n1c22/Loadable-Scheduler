##Test Scheduler

Testing an LKM support based Scheduler. 
The LKM Scheduler primarily works on the principle of load and go.
The scheduler is initiated by compiling the source code associated with the LKM and loading the kernel modules using the scripts.
Once the compiling and loading is complete, the user processes are handed out an option to use the existing OS scheduler or
to use the LKM based scheduler.

###Design of LKM Based Scheduler
- The user processes initially writes its process id to the file `/proc/process_sched_add` which corresponds to the kernel module `process_set`. This procedure completes the registration of a process to the LKM Scheduler.
- The LKM based scheduler is executed internally via the kernel module `process_scheduler`. The module executes a work queue construct for every time quanta.
- The `process_set` and `process_scheduler` modules are coupled through the kernel module `process_queue`. The `process_queue` module handles the internal details of all the processes associated with the LKM Scheduler. It stores the process info as simple link list nodes. 
- Various interfaces are defined within the `process_queue` to perform add, remove, get_first, print operations on the queue. The scheduler performs an add and remove based on the context switch operation being triggered for every time quanta.
- On every time quanta, the scheduler pushes the currently executing PID to the `process_queue` via `add_to_process` interface. And change its execution from Running to wait via `task` based interfaces. Once the currently executing process is added successfully into the queue, the process in the front of the queue is selected. The selected process state is changed to running and also removed from the queue.

###Requirements
- Linux OS with kernel version 3.1x with LKM support enabled.
- `make`
- `gcc` 

###Files included:

- process_set.c - source code for setting a process to the custom scheduler.
- process_scheduler.c - source code for the custom scheduler
- process_queue.c - source code for the process queue maintainance.
- test_pr.c - test process for custom scheduler execution.
- Makefile - For compiling various source code related to the scheduler LKM.
- insmod_scr.sh - LKM insertion script.
- rmmod_scr.sh - LKM removal script.

###Executing the Scheduler
- Open a terminal and go to the location where the git cloning was performed.
- Compile the LKM by running the Makefile. Run `make` to compile the source code related to LKM. If needed a clean build, run `make cleanall` before default `make`.
- After successful compilation of Makefile, you would witness .ko files in the folder. These files are kernel object files for LKM related aspects.
- Now run the script insmod_scr.sh using makefile `make insmod` or you can run `make load` which would compile and load the kernel modules. The execution of above script will only insert the kernel modules process_set, process_scheduler and process_queue to set of kernel modules. After this step the LKM is loaded. You can run `dmesg` to verify if the insertion was successful or not.
- Now compile the test_pr.c source file. This source code can be compiled before the LKM compilation process. But you cannot execute it before the above LKM is loaded. Compilation is done by runnning `make comp_test` or you can compile and run using command `make test`.
- After successful compilation, open two new terminals in the same location and run the test_pr.out in those terminals with the command `./test_pr.out` or `make test`. Note: the test_pr.out will execute infinitely, therefore you will need to terminate it manually.
- Now you can witness the effect of the scheduler within seconds. Currently the scheme used is static round robin scheme with time quantum of 3 secs(default value).
- Finally if you are done using the LKM and you need to remove it run the command `make unload` which would unload the kernel modules and clean them or run the script `make rmmod` which would only remove the kernel module but not clean them.
