#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <sys/utsname.h>

#define NUM_THREADS 128

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


// Unsafe global
int gsum=0;

void *sumThread(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;

	//syslog of current thread and current accumulated sum
        gsum += (int)(threadParams->threadIdx);
        syslog(LOG_CRIT,"[COURSE:1][ASSIGNMENT:2] Thread idx=%d, sum[1...%d]=%d\n", threadParams->threadIdx, threadParams->threadIdx, gsum );
    return NULL;
}


// Main code to start and join threads 
int main (int argc, char *argv[])
{

    struct utsname uts;
    int err = uname(&uts);
    if (err != 0){
      syslog(LOG_CRIT, "error getting system name");
      return err;
    }
    syslog(LOG_CRIT, "%s %s %s %s %s GNU/Linux" ,uts.sysname, uts.nodename, uts.release, uts.version, uts.machine);
    for (int i = 0; i < NUM_THREADS; i++){
       threadParams[i].threadIdx=i;
       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      sumThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   }

   
   for(int i=0; i<NUM_THREADS; i++)
     pthread_join(threads[i], NULL);

   printf("TEST COMPLETE\n");
}
