#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <sys/utsname.h>

#define NUM_THREADS 1

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


void *counterThread(void *threadp)
{
    //int sum=0, i;
	//get params for thread ID
    threadParams_t *threadParams = (threadParams_t *)threadp;

    //for(i=1; i < (threadParams->threadIdx)+1; i++)
    //    sum=sum+i;
	// print hello from thread
    syslog(LOG_CRIT,"[COURSE:1][ASSIGNMENT:1] Hello World from Thread!" );
}


int main (int argc, char *argv[])
{
	int i;
	// get system name
    struct utsname uts;
    int err = uname(&uts);
    if (err != 0){
      syslog(LOG_CRIT, "error getting system name");
      return err;
    }
    
    syslog(LOG_CRIT, "%s %s %s %s %s GNU/Linux" ,uts.sysname, uts.nodename, uts.release, uts.version, uts.machine);
	//print hello from main
    syslog(LOG_CRIT,"[COURSE:1][ASSIGNMENT:1] Hello World from Main!" );
     
	for(i=0; i < NUM_THREADS; i++)
	{
		threadParams[i].threadIdx=i;

		pthread_create(&threads[i],   // pointer to thread descriptor
						(void *)0,     // use default attributes
						counterThread, // thread function entry point
						(void *)&(threadParams[i]) // parameters to pass in
						);

	}

	for(i=0;i<NUM_THREADS;i++)
		pthread_join(threads[i], NULL);

   //printf("TEST COMPLETE\n");
}
