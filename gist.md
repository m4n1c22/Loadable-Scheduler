You could suspend the process by sending it STOP signal, then resume it by sending it a CONT signal; 
perhaps via kill_pid(task_pid(task_struct), SIGSTOP, 1) and kill_pid(task_pid(task_struct), SIGCONT, 1) for example.

##Expectation

- Generate an adaptable design which can handle variable CPU workloads 
- The system should handle an adaptive design which act as a container
