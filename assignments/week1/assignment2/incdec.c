#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <sys/utsname.h>

//#define COUNT  1000
#define NUM_THREADS 128  //define the number of threads

//define struct thread params with thread ID
typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];				// thread descriptor array
threadParams_t threadParams[NUM_THREADS];	// thread params array


// Unsafe global
int gsum=0;

// Summing thread function
void *sumThread(void *threadp)
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp; 	//cast pointer parameters into threadparams
	gsum += (int)(threadParams->threadIdx);						//add ID to current sum
	//syslog of current thread and current accumulated sum
	syslog(LOG_CRIT,"[COURSE:1][ASSIGNMENT:2] Thread idx=%d, sum[1...%d]=%d\n", threadParams->threadIdx, threadParams->threadIdx, gsum );
	//printf("Thread idx=%d, sum[1...%d]=%d\n", threadParams->threadIdx, threadParams->threadIdx, gsum);
}

// Main code to start and join threads 
int main (int argc, char *argv[])
{
	// Get system name and print it to syslog
	struct utsname uts;
	int err = uname(&uts);
	if (err != 0){
	  syslog(LOG_CRIT, "error getting system name");
	  return err;
	}
	syslog(LOG_CRIT, "%s %s %s %s %s GNU/Linux" ,uts.sysname, uts.nodename, uts.release, uts.version, uts.machine);
	
	// create threads
	for (int i = 0; i < NUM_THREADS; i++){
	   threadParams[i].threadIdx=i;
	   pthread_create(&threads[i],   // pointer to thread descriptor
					  (void *)0,     // use default attributes
					  sumThread, // thread function entry point
					  (void *)&(threadParams[i]) // parameters to pass in
					 );
	}

	// wait for completition and join threads
	for(int i=0; i<NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	// print when done
	printf("TEST COMPLETE\n");
}
