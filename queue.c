////////////////////////////////////////////////////////////////////////
// COMP2521 19T0 -- A Queue ADT implementation, using linked lists.
//
// 2018-12-01	Jashank Jeremy <jashankj@cse.unsw.edu.au>
// YYYY-mm-dd	Your Name Here <zNNNNNNN@student.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "queue.h"

typedef struct queue queue;
typedef struct queue_node queue_node;


static queue_node *queue_node_new (location_t loc);

/** Create a new, empty Queue. */
queue *queue_new (void)
{
	queue *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate queue");
	(*new) = (queue) { .n_locations = 0, .max_locations = 6, .head = NULL, .tail = NULL };
	return new;
}

/** Destroy a Queue, releasing all resources associated with it. */
void queue_drop (queue *q)
{
    //blackbox & whitebox tests broke here as we never tested whehter the queue was empty
    // also, the logic was wrong, as we would first free a node then try to access
    // the next node using the freed node (i.e using head after stack)
	assert (q != NULL);
    queue_node *curr, *rem;
    curr = q->head;
    
    while (curr != NULL) {
	    rem = curr;
        curr = curr->next;
        free(rem);
    }
    free (q);
}

/** Add an item to the end of a Queue.
 * Sometimes referred to as "enqueue" or "unshift". */
// whitebox testing broke code from this function 
void queue_en (queue *q, location_t location)
{
	queue_node *node = queue_node_new (location);
	// changed true condition to add to tail and update tail
	if (q->head != NULL) {
		q->tail->next = node;
		q->tail = q->tail->next;
		q->tail->next = NULL;
	// changed false condition to make the head the node and make the tail point to the head.
	} else {
		q->head = node;
	    q->tail = q->head;
	}
	// added increment to q->n_items
	q->n_locations++;
}

/** Remove an item from the front of a Queue.
 * Sometimes referred to as "dequeue" or "shift". */
 // whitebox tests broke code at this function as well
location_t queue_de (queue *q)
{
    assert (q != NULL);
    if (q->head == NULL) {
		fprintf (stderr, "queue underflow!\n");
		abort();
	}
	location_t loc = q->head->location;
	queue_node *del = q->head;
	q->head = q->head->next;
	// added subtraction to n_items;
	free (del);
	q->n_locations--;
	return loc;
}

/** Get the number of items in a Queue. */
size_t queue_size (queue *q)
{
	assert (q != NULL);
	return q->n_locations;
}

static queue_node *queue_node_new (location_t loc)
{
	queue_node *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate queue_node");
	(*new) = (queue_node) { .location = loc, .next = NULL };
	return new;
}

// what bugs i found using whitebox testing
// 1 - enque wasnt enqueuing in correct order and wasnt incrementing n_items
// 2 - deque didnt check for empty lists and didnt decrement n_items
// 3 - queue_drop was attempting to us a free node to access the next node in the list
// 
void white_box_tests (void)
{
	/*// making sure the queue isnt null
	queue *new = queue_new();
	//making sure the queue starts of with zero items
	assert(new->n_items == 0);

	Item array[3] = {1,2,3};
	for(int i = 0; i < 3; i++) {
		queue_en(new, array[i]);
	}
	//checking whether enqueue function increments number of items correctly
	assert(new->n_items == 3);
	// checking whether enqueue adds the head correctly
	assert(new->head->item == 1);
	// checking whether enqueue adds an intermediate item correctly
	assert(new->head->next->item == 2);
	// checking whether enqueue adds the tail correctyly
	assert(new->tail->item == 3);
	// checking whether dequeeu removes the correct node
	assert(queue_de(new) == 1);
	assert(queue_de(new) == 2);
	
	// to check abort in deque
	//assert(queue_de(new) == 3);
	//assert(queue_de(new) == 0);
	//checking wehterh dequeue subtracts from n_items correctly
	assert(new->n_items == 1);
	// ... you need to write theseves the !
	
	queue_drop(new);*/
}
