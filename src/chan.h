/* file: chan.h -- public interface to message passingfunctions. */

#ifndef CHAN_DEFINED
#define CHAN_DEFINED

#include "sem.h"
#include "thread.h"

typedef struct {
        Sem *rblock; /* receivers block on this */
        Sem *sblock; /* senders block on this */
        Sem *send_serialiser; /* to prevent concurrent senders clashing */
        int data;
} Chan;

Chan *chan_create(void);
int chan_destroy(Chan *chan);
void chan_send(Chan *chan, int sentdata);
void chan_receive(Chan *chan, int *receiveddata);

#endif
/* end file: chan.h */
