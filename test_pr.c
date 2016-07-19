#include<stdio.h>
#include<stdlib.h>
int main() {
 FILE *fp = fopen("/proc/process_sched_add","w");
 fprintf(fp, "%d", getpid());
 fclose(fp);
}
