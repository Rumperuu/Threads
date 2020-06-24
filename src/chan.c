/************************************************************************
 *                                                                      *
 * file: chan.c                                                         *
 *                                                                      *
 * Module implementing message passing IPC for threads.                 *
 *                                                                      *
 * Our implementation assumes that threads calling us are               *
 * subject to arbitrary preemption.                                     *
 *                                                                      *
 ************************************************************************/

#include "thread.h"
#include "chan.h"
#include <stdlib.h> 
#include <stdio.h> 

Sem* chan_mutex;  /* protect chan module from race conditions */

/************************************************************************
 *  INTERFACE: chan_create()                                            *
 *                                                                      *
 * Make a channel and return a pointer to it. Return (Chan *)0 on       *
 * failure. A channel is a struct which we allocate using malloc().     *
 * Internally, the channel consists of 3 semaphores which are created   *
 * using chan_create(). We initalise the data field of the channel to 0.*
 * Failure occurs if either the malloc() fails, or we fail to create    *
 * some of the semaphores. In case of failure we are careful to         *
 * free any just-allocated resources to avoid memory leaks.             *
 ************************************************************************/
Chan*
chan_create()
{
       	Chan* c;
	static int firstCall = 1;

	if (firstCall) {
		firstCall = 0;
		if((chan_mutex = sem_create(1)) == (Sem*)0)
			return (Chan*)0;
	}

	if ((c = (Chan*)malloc(sizeof(Chan))) == (Chan*)0)
		return (Chan*)0;

	if ((c->sblock = sem_create(0)) == (Sem*)0) {
		free((void*)c);
		return (Chan*)0;
	}

	if ((c->rblock = sem_create(0)) == (Sem*)0) {
		free((void*)c);
		sem_destroy(c->sblock);
		return (Chan*)0;
	}

	if ((c->send_serialiser = sem_create(1)) == (Sem*)0) {
		free((void*)c);
		sem_destroy(c->sblock);
		sem_destroy(c->rblock);
		return (Chan*)0;
	}

	c->data = 0;
	return c;
}

/************************************************************************
 *  INTERFACE: chan_destroy()                                           *
 *                                                                      *
 * Destroy a channel. Must only be allowed to succeed if we are able    *
 * to successfully destroy *all* the semaphores. Must handle the case   *
 * where we can destroy some semaphores but not others (in which case   *
 * we should obviously report failure). If we fail to destroy           *
 * a semaphore, we need to restore previously-destroyed ones to return  *
 * the channel to its pre-call state.                                   *
 * Return -1 on failure; and 1 on success.                              *
 ************************************************************************/
int
chan_destroy(Chan* c)
{
	Chan* backup;

	sem_P(chan_mutex);
	if ((backup = chan_create()) == (Chan*)0)
	  	return -1;
	
	if ((sem_destroy(c->rblock)) == -1) {
		sem_destroy(backup->rblock);
		sem_destroy(backup->sblock);
		sem_destroy(backup->send_serialiser);
		free((void*)backup);
		sem_V(chan_mutex);
		return -1;
	}
	/* So far: successfully destroyed rblock */
	if ((sem_destroy(c->sblock)) == -1) {
		c->rblock = backup->rblock;
 		sem_destroy(backup->sblock);
		sem_destroy(backup->send_serialiser);
		free((void*)backup);
		sem_V(chan_mutex);
		return -1;
	}
	
	/* So far: successfully destrotyed rblcok and sblock */
	if ((sem_destroy(c->send_serialiser)) == -1) {
		c->rblock = backup->rblock;
		c->sblock = backup->sblock;
		sem_destroy(backup->send_serialiser);
		free((void*)backup);
		sem_V(chan_mutex);
		return -1;
	}

	/* So far: successfully destroyed rblock, sblock and send_serialiser */
	sem_destroy(backup->rblock);
	sem_destroy(backup->sblock);
	sem_destroy(backup->send_serialiser);
	free((void*)backup);
	free((void*)c);
	sem_V(chan_mutex);
	return 1;
}

/************************************************************************
 *  INTERFACE: chan_send()                                              *
 *                                                                      *
 * Send a message on a channel.                                         *
 * This will involve, first, attaching the given int 'message' to the   *
 * channel, then telling chan_receive() that we have a message to send  *
 * (by calling sem_V() on one sem), and then waiting (by calling sem_P()*
 * on the other sem) until chan_receive() has taken the message and     *
 * told us we can proceed. We can use the send_serialiser semaphore to  *
 * ensure that concurrent calls to chan_send() are serialised.          *
 ************************************************************************/
void 
chan_send(Chan* c, int sentdata) 
{ 
	sem_P(c->send_serialiser);
	c->data = sentdata;
	sem_V(c->rblock);
	sem_P(c->sblock);
	sem_V(c->send_serialiser);
}

/************************************************************************
 *  INTERFACE: chan_receive()                                           *
 *                                                                      *
 * Receive a message on a channel.                                      *
 * This will involve, first, waiting on one semaphore (using sem_P())   *
 * until chan_send() has given us something to receive; then making     *
 * receiveddata point at the message in the channel; then               *
 * telling chan_send() that it may proceed (using sem_V() on the other  *
 * semaphore).                                                          *
 ************************************************************************/
void 
chan_receive(Chan* c, int* receiveddata)
{
	sem_P(c->rblock);
	*receiveddata = c->data;
	sem_V(c->sblock);
}

/* end file: chan.c */
