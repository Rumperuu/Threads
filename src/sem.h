/* file: sem.h -- definitions for semaphores. */

#ifndef SEM_DEFINED
#define SEM_DEFINED

/* Include public interface. */
#include "queue.h"
#include "thread.h"

typedef struct semtype {
        int val;
        Queue queue;
} Sem;

Sem *sem_create(int val);
int sem_destroy(Sem *s);
void sem_P(Sem *s);
void sem_V(Sem *s);

#endif
/* end file: sem.h */
