/************************************************************************
 *                                                                      *
 * file: sem.c                                                          *
 *                                                                      *
 * Module implementing semaphores.                                      *
 *                                                                      *
 * All routines should call thread_yield() where reasonable to encourage*
 * fair execution.                                                      *
 *                                                                      *
 * Note that our implementation assumes that threads calling us are not *
 * subject to arbitrary preemption - if they were, we would need to     *
 * add a protecting spin lock to our semaphore implementation.          *
 *                                                                      *
 ************************************************************************/

#include "sem.h"
#include <stdlib.h> 
#include <stdio.h> 

/* External stuff defined in thread.c.
 */
extern Queue ready_queue;
extern int thread_block_and_switch(Queue* q);

/************************************************************************
 *  INTERFACE: sem_create()                                             *
 *                                                                      *
 * Make and return a semaphore (i.e. return a pointer to a Sem struct). *
 * This will involve allocating a Sem structure using malloc(),         *
 * creating/initialising a queue for threads blocked on the             *
 * semaphore and also initialising the semaphore to the value specified *
 * by our caller. This value must be validated to be >= 0. The          *
 * function can only fail if malloc() fails - in this case we return    *
 * (Sem *)0.                                                            *
 ************************************************************************/
Sem* sem_create(int val)
{
	Sem* s;

	if (val < 0 || ((s = (Sem*)malloc(sizeof(Sem))) == (Sem*)0))
	  	return (Sem*)0;

	s->val = val;
	queue_init(&(s->queue));
	thread_yield();
	return s;
}

/************************************************************************
 *  INTERFACE: sem_destroy()                                            *
 *                                                                      *
 * Destroy a semaphore.                                                 *
 * Forbid destruction (return -1) if the semaphore has waiting threads. *
 * Otherwise return 1 for success.                                      *
 ************************************************************************/
int 
sem_destroy(Sem* s)
{
	if (!queue_empty(&(s->queue))) return -1;
	free((void*)s);
	return 1;
}

/************************************************************************
 *  INTERFACE: sem_P()                                                  *
 *                                                                      *
 * Standard semaphore operation.                                        *
 * That is, decrement the value of the semaphore and if this takes      *
 * its value below 0 block the calling thread on the semaphore's queue  *
 * using thread_block_and_switch().                                     *
 * We know that a semaphore value can't theoretically get less than 0,  *
 * but we internally (hidden from the semaphore user) use the negative  *
 * range of val to record how many threads are currently blocked on the *
 * semaphore.                                                           *
 ************************************************************************/
void 
sem_P(Sem* s)
{
	if (--s->val < 0) thread_block_and_switch(&(s->queue));
	else thread_yield();
}

/************************************************************************
 *  INTERFACE: sem_V()                                                  *
 *                                                                      *
 * Standard semaphore operation.                                        *
 * That is, increment the value of the semaphore. If there were threads *
 * blocked on the semaphore (i.e. the value after incrementing the      *
 * semaphore is still <= 0), we get the first thread from the           *
 * semaphore's queue and put it on the thread package's ready queue     *
 * using queue_put().                                                   *
 * Because the sem module is solely responsible for managing a sem's    *
 * queue, we can guarantee that if (s->val <= 0) there is always an     *
 * item on the sem's queue.                                             *
 ************************************************************************/
void 
sem_V(Sem *s)
{
	if (++s->val <= 0) queue_put(&ready_queue, queue_get(&(s->queue)));
	thread_yield();
}

/* end file: sem.c */
