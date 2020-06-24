/* file: sem.h -- definitions for semaphores. */

#ifndef SEM_DEFINED
#define SEM_DEFINED

/* Include public interface. */
#include "../thread.h"
#include <pthread.h>

typedef struct semtype {
        int val;
	pthread_cond_t cond;
} Sem;

Sem *create_sem(int val);
int destroy_sem(Sem *s);
void sem_P(Sem *s);
void sem_V(Sem *s);

#endif
/* end file: sem.h */
