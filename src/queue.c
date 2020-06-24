/************************************************************************
 *                                                                      *
 * file: queue.c                                                        *
 *                                                                      *
 * Module implementing queue manipulation functions. The module can     *
 * hold structs of any type as long as the user of the module           *
 * does the appropriate casting -- all it assumes is that the first     *
 * member of the struct is a pointer, 'next', to a struct.              *
 *                                                                      *
 ************************************************************************/

#include "queue.h"
#include <stdio.h>

/************************************************************************
 * queue_init()                                                         *
 *                                                                      *
 * Initialise a Queue struct.                                           *
 * We assume this will never fail - so always return 1 for success.     *
 ************************************************************************/
int
queue_init(Queue* q)
{
	q->front = q->back = (Qitem*)0;
	return 1;
}

/************************************************************************
 * queue_empty()                                                        *
 *                                                                      *
 * Return TRUE (i.e. non-zero) iff queue is empty; else FALSE (i.e. 0). *
 ************************************************************************/
int
queue_empty(Queue* q)
{
	return q->front == (Qitem*)0;
}

/************************************************************************
 * queue_put()                                                          *
 *                                                                      *
 * Add a new Qitem to the back of the specified queue.                  *
 * Make sure the next pointer of the new Qitem is 0!                    *
 * We assume the given Qitem can be used directly by us and does not    *
 * need to be copied.                                                   *
 ************************************************************************/
void
queue_put(Queue* q, Qitem* new_item)
{
	new_item->next = (Qitem*)0;
	if (queue_empty(q)) 
		q->front = new_item;
	else
		q->back->next = new_item;
	q->back = new_item;
}

/************************************************************************
 * queue_get()                                                          *
 *                                                                      *
 * Remove the front Qitem struct from the specified queue and return    *
 * a pointer to it. When we remove and return the struct we pass all    *
 * future responsibility for it to our caller. Return (Qitem *)0 if     *
 * queue is empty.                                                      *
 ************************************************************************/
Qitem*
queue_get(Queue* q)
{
	Qitem* p = q->front;
	if (!queue_empty(q)) {
		q->front = q->front->next;
		if (q->front == (Qitem*)0) q->back = (Qitem*)0;
	}
	return p;
}

/* end file: queue.c */


