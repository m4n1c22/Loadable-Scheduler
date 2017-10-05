/**
	\file	:	test_pr.c
	\author	: 	Sreeram Sadasivam
	\brief	:	Test process program for evaluating its compactibility with the custom scheduler.
*/

#include <stdio.h>
#include <stdlib.h>

void test_pr(void) {

	FILE *fp = fopen("/proc/process_sched_add","w");
	fprintf(fp, "%d", getpid());
 	fclose(fp);

 	while(1) {
 		printf("My pid: %d\n", getpid());
 		sleep(1);
 	}
}

int main() {
    test_pr();
	return 0;
}
