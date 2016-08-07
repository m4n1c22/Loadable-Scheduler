##Test Scheduler

Testing an LKM support based Scheduler.

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
- Compile the LKM by running the Makefile. Run `make` to compile the source code related to LKM. If needed a clean build, run `make clean` before default `make`.
- After successful compilation of Makefile, you would witness .ko files in the folder. These files are kernel object files for LKM related aspects.
- Now run the script insmod_scr.sh as `sh insmod_scr.sh`. The execution of above script will insert the kernel modules process_set, process_scheduler and process_queue to set of kernel modules. After this step the LKM is loaded. You can run `dmesg` to verify if the insertion was successful or not.
- Now compile the test_pr.c source file. This source code can be compiled before the LKM compilation process. But you cannot execute it before the above LKM is loaded. Compilation is done by runnning `gcc test_pr.c -o test_pr.out`
- After successful compilation, open two new terminals in the same location and run the test_pr.out in those terminals with the command `./test_pr.out`. Note: the test_pr.out will execute infinitely, therefore you will need to terminate it manually.
- Now you can witness the effect of the scheduler after few seconds. It will take time for the processes to get registered to the scheduler. Potentially less than the time quantum defined for the scheduling scheme. Currently the scheme used is fixed round robin scheme with time quantum of 10 secs.
