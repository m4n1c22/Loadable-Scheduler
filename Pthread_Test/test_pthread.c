/**
  \file : test_pthread.c
  \author :   Sreeram Sadasivam
  \brief  : Test pthread program for evaluating its compactibility with the custom scheduler.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>



/**Macros*/
#define NUM_THREADS   2


void *test_pthread(void *ptr) {


  #ifdef SYS_gettid
  pid_t tid = syscall(SYS_gettid);
  #else
  #error "SYS_gettid unavailable on this system"
  #endif
  FILE *fp = fopen("/proc/process_sched_add","w");
  fprintf(fp, "%d", tid);
  fclose(fp);

  while(1) {
    printf("My tid: %d\n", tid);
    sleep(1);
  }
  pthread_exit(NULL);
}

int main() {

  pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for(t=0; t<NUM_THREADS; t++){
       printf("In main: creating thread %ld\n", t);
       rc = pthread_create(&threads[t], NULL, test_pthread, NULL);
       if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
       }
    }
    /* Last thing that main() should do */
    pthread_exit(NULL);
  return 0;
}
