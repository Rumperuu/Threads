/************************************************************************
 *                                                                      *
 * file: sem.c                                                          *
 *                                                                      *
 * Module implementing semaphores.                                      *
 *                                                                      *
 ************************************************************************/

#include "sem.h"
#include <stdlib.h> 

pthread_mutex_t sem_mutex; /* protect sem module from race conditions */

/************************************************************************
 *  INTERFACE: sem_create()                                             *
 *                                                                      *
 * Make and return a semaphore (i.e. return a pointer to a Sem struct). *
 ************************************************************************/
Sem *sem_create(int val)
{
        Sem *s;
        static int firstcall = 1;

	/* We, perhaps naively, assume that sem_create() will not be 
	 * called concurrently!
	 */
        if (firstcall) {
                firstcall = 0;
                pthread_mutex_init(&sem_mutex, NULL);
        }

        if (val < 0) return (Sem *)0;

        pthread_mutex_lock(&sem_mutex);

        if ((s = (Sem *)malloc(sizeof(Sem))) == (Sem *)0) {
                return (Sem *)0;
        }
        s->val = val; 
        pthread_cond_init(&s->cond, NULL);

        pthread_mutex_unlock(&sem_mutex);

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
sem_destroy(Sem *s)
{
        pthread_mutex_lock(&sem_mutex);

        if (s->val < 0) {
                /* Mustn't destroy a semaphore with waiting threads
                 * on it.
                 */

                pthread_mutex_unlock(&sem_mutex);
                return -1;
        }
          pthread_cond_destroy(&s->cond);
        free((void *)s);

        pthread_mutex_unlock(&sem_mutex);

        return 1;
}

/************************************************************************
 *  INTERFACE: sem_P()                                                  *
 *                                                                      *
 * Do a P on a semaphore(!)                                             *
 ************************************************************************/
void 
sem_P(Sem *s)
{
        pthread_mutex_lock(&sem_mutex);

        if (--s->val < 0) {
                /* wait on the semaphore's condition */
                pthread_cond_wait(&s->cond, &sem_mutex);
        }
        
        pthread_mutex_unlock(&sem_mutex);
}

/************************************************************************
 *  INTERFACE: sem_V()                                                  *
 *                                                                      *
 * Do a V on a semaphore.                                               *
 ************************************************************************/
void 
sem_V(Sem *s)
{
        pthread_mutex_lock(&sem_mutex);

        if (++s->val <= 0) {
                /* assert the semaphore's condition */
                      pthread_cond_signal(&s->cond);
        }
        
        pthread_mutex_unlock(&sem_mutex);

}

/* end file: sem.c */
