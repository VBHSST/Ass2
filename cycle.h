#include <stdlib.h>
#include "places.h"

#ifndef CS2521__CYCLE_H_
#define CS2521__CYCLE_H_
typedef enum location location_t;
struct link {
    location_t location;
    struct link *next;
	struct link *prev;
};

struct cycle {
    struct link *top;
	struct link *bottom;
    int n_filled;
    int total;
};

typedef struct cycle *Cycle;
typedef struct link *Link;

Cycle cycle_init (void);

Cycle add_link(Cycle c);

void cycle_push(Cycle c, location_t location);

void cycle_pop_bottom(Cycle c);

void cycle_print(Cycle c);

void cycle_drop(Cycle c);

#endif // !defined(CS2521__CYCLE_H_)
