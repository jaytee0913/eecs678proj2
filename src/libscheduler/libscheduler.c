/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
  int job_number;
  int time_start;
  int running_time;
  int priority;
  int time_stop;
  int just_switched;
  int core_number;
} job_t;

typedef struct _core_t
{
  int core_number;
  int available;
  job_t *job;
} core_t;

priqueue_t j_queue;
priqueue_t c_queue;
int time_current;
int fcfs  (const void* a, const void* b);
int sjf   (const void* a, const void* b);
int psjf  (const void* a, const void* b);
int pri   (const void* a, const void* b);
int ppri  (const void* a, const void* b);
int rr    (const void* a, const void* b);

int fcfs(const void* a, const void* b)
{
  return 1;
}

int sjf(const void* a, const void* b)
{
  job_t* a_temp = (job_t*)a;
  job_t* b_temp = (job_t*)b;
  if(b_temp->core_number > -1)
  {
    return 1;
  }
  else
  {
    return a_temp->running_time - b_temp->running_time;
  }
}

int psjf(const void* a, const void* b)
{
  job_t* a_temp = (job_t*)a;
  job_t* b_temp = (job_t*)b;
  if(b_temp->time_stop >= b_temp->time_start)
  {
    return a_temp->running_time - b_temp->running_time;
  }
  if(b_temp->time_start > -1)
  {
    return a_temp->running_time - (b_temp->running_time - (time_current - b_temp->time_start));
  }
  return a_temp->running_time - b_temp->running_time;
}

int pri(const void* a, const void* b)
{
  job_t* a_temp = (job_t*)a;
  job_t* b_temp = (job_t*)b;
  if(b_temp->core_number > -1)
  {
    return 1;
  }
  return a_temp->priority - b_temp->priority;
}

int ppri(const void* a, const void* b)
{
  job_t* a_temp = (job_t*)a;
  job_t* b_temp = (job_t*)b;
  return a_temp->priority - b_temp->priority;
}

int rr(const void* a, const void* b)
{
  return 1;
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  priqueue_init(&c_queue, NULL);
  for(int i = 0; i < cores; i++)
  {
    core_t *core = malloc(sizeof(core_t));
    core->core_number = i;
    core->available = 1;
    core->job = NULL;
    priqueue_offer(&c_queue, core);
  }

  switch(scheme)
  {
    case FCFS:
      priqueue_init(&j_queue, fcfs);
      break;
    case SJF:
      priqueue_init(&j_queue, sjf);
      break;
    case PSJF:
      priqueue_init(&j_queue, psjf);
      break;
    case PRI:
      priqueue_init(&j_queue, pri);
      break;
    case PPRI:
      priqueue_init(&j_queue, ppri);
      break;
    case RR:
      priqueue_init(&j_queue, rr);
      break;
  }
  j_queue.ptr_flag = 1;
  c_queue.ptr_flag = 1;
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  time_current = time;
  job_t* job = malloc(sizeof(job_t));
  job->job_number = job_number;
  job->time_start = -1;
  job->running_time = running_time;
  job->priority = priority;
  job->time_stop = -1;
  int x = priqueue_offer(&j_queue, job);
  node *i = c_queue.head;
  while(i)
  {
    core_t *core = i->data;
    if(core->available)
    {
      core->available = 0;
      core->job = job;
      job->core_number = core->core_number;
      job->time_start = time;
      return core->core_number;
    }
    i = i->next;
  }
	
  i = priqueue_node_at(&j_queue, x);
  if(i->next)
  {
    job_t *job_temp = i->next->data;
    if(job_temp->core_number > -1)
    {
      i = i->next;
      node *i_prev = i;
      while(i)
      {
        job_temp = i->data;
        if(job_temp->core_number < 0)
        {
          break;
        }
        i_prev = i;
        i = i->next;
      }
      job_temp = i_prev->data;
      job->core_number = job_temp->core_number;
      job_temp->core_number = -1;
      job->time_start = time;
      if(job_temp->time_start > -1)
      {
        job_temp->time_stop = time;
        job_temp->running_time -= (job_temp->time_stop - job_temp->time_start);
      }
      i = priqueue_node_at(&c_queue, job->core_number);
      core_t *core = i->data;
      core->job = job;
      return core->core_number;
    }
    else
    {
      job->core_number = -1;
      return -1;
    }
  }
  job->core_number = -1;
  return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  time_current = time;
  node *i = priqueue_node_at(&c_queue, core_id);
  core_t *core = i->data;
  core->available = 1;
  priqueue_remove(&j_queue, core->job);
  free(core->job);
  core->job = NULL;

  i = j_queue.head;
  while(i)
  {
    job_t *job = i->data;
    if(job->core_number < 0)
    {
      core->available = 0;
      core->job = job;
      job->core_number = core->core_number;
      job->time_start = time;
      return job->job_number;
    }
    i = i->next;
  }
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
  time_current = time;
  core_t *core = priqueue_at(&c_queue, core_id);
  job_t *job_exp = core->job;
  priqueue_remove(&j_queue, job_exp);
  priqueue_offer(&j_queue, job_exp);
  node *i = j_queue.head;
  while(i)
  {
    job_t *job = i->data;
    if(job->core_number < 0)
    {
      job->core_number = job_exp->core_number;
      job_exp->core_number = -1;
      core->job = job;
      return job->job_number;
    }
    i = i->next;
  }
  return job_exp->job_number;
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  priqueue_destroy(&j_queue);
  priqueue_destroy(&c_queue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  node *i = j_queue.head;
  while(i){
    job_t *job = i->data;
    printf("%d(%d) ", job->job_number, job->core_number);
    i = i->next;
  }
  printf("\n");
}
