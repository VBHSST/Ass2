#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "cycle.h"

typedef struct cycle cycle;
typedef struct link link;

cycle *cycle_init (void) {
    cycle *c = malloc(sizeof(struct cycle));
	if (c == NULL) err (EX_OSERR, "couldn't allocate cycle");
	(*c) = (cycle) {.top = NULL, .bottom = NULL, .total = 6, .n_filled = 0};

	for (int i = 0; i < 6; i++) {
   		c = add_link(c);
	}	
	c->n_filled = 0;
    return c;
}

cycle *add_link(Cycle c) {
    link *l = malloc(sizeof(struct link));
	if (c == NULL) err (EX_OSERR, "couldn't allocate link");
	(*l) = (link) { .prev = c->bottom, .next = c->top, .location = -1};

	if (c->n_filled == 0) {
		c->top = l;
		c->bottom = l;
	} else {
		c->top->prev = l;
		c->bottom->next = l;
    	c->top = l;
	}
	c->n_filled++;
    return c;
}


void cycle_push(Cycle c, location_t location) {
	if (c->n_filled == c->total) {
		cycle_pop_bottom(c);
	} 

	c->top->prev->location = location;
	c->top = c->top->prev;
	c->n_filled++;
}

void cycle_pop_bottom(Cycle c) {
	assert(c->n_filled != 0);
	c->bottom->location = -1;
	c->bottom = c->bottom->prev;

	c->n_filled--;	
}

void cycle_print(Cycle c) {
	int i = 0;
	link *curr = c->top;
	do {
		if (curr == c->top) {
			printf ("top: %d, value = %d\n", i, curr->location);
		} else if (curr == c->bottom) {
			printf ("bottom: %d, value = %d\n", i, curr->location);
		} else {
			printf ("link: %d, value = %d\n", i, curr->location);
		}
		i++;
		curr = curr->next;
	} while (curr != c->top);
}

void cycle_drop(Cycle c) {
	c->bottom->next =  NULL;
	c->top->prev = NULL;
	
	link *curr = c->top;
	while (curr != NULL) {
		Link temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(c);
}

