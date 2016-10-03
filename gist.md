#Task Status Change Construct Gist:
You could suspend the process by sending it STOP signal, then resume it by sending it a CONT signal; 
perhaps via kill_pid(task_pid(task_struct), SIGSTOP, 1) and kill_pid(task_pid(task_struct), SIGCONT, 1) for example.

##Expectation

- Generate an adaptable design which can handle variable CPU workloads. 
- The system should provide an adaptive design which act as a container class/structure for the problem.
- Potential Cloning possibility of the scheduler module
- Adding Dynamic Round Robin Scheme


Code.

#include <sys/types.h>
#include <sys/times.h>

struct tb { float usrtime; float systime; };

time_t dutime=0, dstime=0;

float
dtime(dt) struct tb *dt;
{	struct tms clock;

	times(&clock);
	dt->usrtime = (float)(clock.tms_utime - dutime) / 60.0;
	dt->systime = (float)(clock.tms_stime - dstime) / 60.0;
	dutime = clock.tms_utime;
	dstime = clock.tms_stime;
	return(dt->usrtime + dt->systime);
}
