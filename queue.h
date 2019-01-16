#include <stdlib.h>
#include "places.h"

#ifndef CS2521__QUEUE_H_
#define CS2521__QUEUE_H_


typedef struct queue *Queue;
typedef struct queue_node queue_node;

struct queue_node {
	location_t location;
	queue_node *next;
	//turn number
};
struct queue {
    size_t n_locations;
	size_t max_locations;
	queue_node *head, *tail;
};


/** Create a new, empty Queue. */
Queue queue_new (void);

/** Destroy a Queue, releasing all resources associated with it. */
void queue_drop (Queue);

/** Remove an item from the front of a Queue.
 * Sometimes referred to as "dequeue" or "shift". */
location_t queue_de (Queue q);

/** Add an item to the end of a Queue.
 * Sometimes referred to as "enqueue" or "unshift". */
void queue_en (Queue, location_t location);

/** Get the number of items in a Queue. */
size_t queue_size (Queue);

void white_box_tests(void);

#endif // !defined(CS2521__QUEUE_H_)
