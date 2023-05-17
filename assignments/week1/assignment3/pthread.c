#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <syslog.h>
#include <sys/utsname.h>

#define NUM_THREADS 128
#define NUM_CPUS 4


// Course number
#define COURSE_NO "1"
// Assignment number
#define ASSIGNMENT_NO "3"
// Macro which sends message to syslog with format required by course
#define SP_SYSLOG(...) syslog(LOG_INFO,"[COURSE:" COURSE_NO "][ASSIGNMENT:" ASSIGNMENT_NO "] " __VA_ARGS__)

typedef struct
{
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
pthread_t mainthread;
pthread_t startthread;
threadParams_t threadParams[NUM_THREADS];

pthread_attr_t fifo_sched_attr;
pthread_attr_t orig_sched_attr;
struct sched_param fifo_param;

#define SCHED_POLICY SCHED_FIFO
#define MAX_ITERATIONS (1000000)


void print_scheduler(void)
{
    int schedType = sched_getscheduler(getpid());

    switch(schedType)
    {
        case SCHED_FIFO:
            printf("Pthread policy is SCHED_FIFO\n");
            break;
        case SCHED_OTHER:
            printf("Pthread policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf("Pthread policy is SCHED_RR\n");
            break;
        default:
            printf("Pthread policy is UNKNOWN\n");
    }
}


void set_scheduler(void)
{
    int max_prio, scope, rc, cpuidx;
    cpu_set_t cpuset;

    printf("INITIAL "); print_scheduler();
    // initialize attributes
    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);
    // se affinity to cpu = 3
    CPU_ZERO(&cpuset);
    cpuidx=(3);
    CPU_SET(cpuidx, &cpuset);
    pthread_attr_setaffinity_np(&fifo_sched_attr, sizeof(cpu_set_t), &cpuset);

    // max priority
    max_prio=sched_get_priority_max(SCHED_POLICY);
    fifo_param.sched_priority=max_prio;         

    if((rc=sched_setscheduler(getpid(), SCHED_POLICY, &fifo_param)) < 0)
        perror("sched_setscheduler");
    // set as fifo
    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);

    printf("ADJUSTED "); print_scheduler();
}



// Thread function
void *thread_func(void *threadp)
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp;
	int sum = 0;
	// Arithmetic progression [1..idx] Calculating
    for(i = 1; i <= threadParams->threadIdx; i++)
    {
        sum = sum + i;
    }
	// Print result of arithmetic progression
	SP_SYSLOG("Thread idx=%d, sum[0...%d]=%d Running on core:%d", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu());

}


    // start threads from a thread
void *starterThread(void *threadp)
{
   int i, rc;

   printf("starter thread running on CPU=%d\n", sched_getcpu());

   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;
        // create each thread and set the attibutes
       pthread_create(&threads[i],   // pointer to thread descriptor
                      &fifo_sched_attr,     // use FIFO RT max priority attributes
                      thread_func, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );

   }
     //wait for all threads to join
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

}


int main (int argc, char *argv[])
{
    
   // Get system name and version
   struct utsname uname_st;
   uname(&uname_st);
   // Open syslog
   openlog("pthread",0,LOG_LOCAL0);
   // Print system information to syslog
   SP_SYSLOG("%s %s %s %s %s GNU/Linux",
		   uname_st.sysname, uname_st.nodename, uname_st.release, uname_st.version, uname_st.machine);
           
   int rc;
   int i, j;
   cpu_set_t cpuset;

   set_scheduler();

   CPU_ZERO(&cpuset);

   // get affinity set for main thread
   mainthread = pthread_self();

   // Check the affinity mask assigned to the thread 
   rc = pthread_getaffinity_np(mainthread, sizeof(cpu_set_t), &cpuset);
   if (rc != 0)
       perror("pthread_getaffinity_np");
   else
   {
       printf("main thread running on CPU=%d, CPUs =", sched_getcpu());

       for (j = 0; j < CPU_SETSIZE; j++)
           if (CPU_ISSET(j, &cpuset))
               printf(" %d", j);

       printf("\n");
   }
    // create starter thread
   pthread_create(&startthread,   // pointer to thread descriptor
                  &fifo_sched_attr,     // use FIFO RT max priority attributes
                  starterThread, // thread function entry point
                  (void *)0 // parameters to pass in
                 );

   pthread_join(startthread, NULL);

   printf("\nTEST COMPLETE\n");
}
