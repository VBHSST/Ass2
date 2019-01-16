////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.h: an interface to a Map data type
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>

#include <stdbool.h>
#include <stdlib.h>

#include "places.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

typedef struct edge {
    location_t start, end;
    transport_t type;
} edge;

struct map {
	size_t n_vertices, n_edges;

	struct map_adj {
		location_t v;  // ALICANTE, etc
		transport_t type; // ROAD, RAIL, BOAT
		struct map_adj *next; // link to next node
	} *connections[NUM_MAP_LOCATIONS]; // array of lists
};

// graph representation is hidden
typedef struct map *Map;
typedef struct map_adj map_adj;

/** Create a new Map. */
Map map_new (void);
/** Release resources associated with a Map. */
void map_drop (Map);
/** Print a Map to `stdout`. */
void map_show (Map);
/** Get the number of vertices. */
size_t map_nv (Map);
/** Get the number of edges. */
size_t map_ne (Map, transport_t);

void add_connections (Map);
void add_connection (Map, location_t, location_t, transport_t);
bool is_sentinel_edge (connection);

map_adj *adjlist_insert (map_adj *, location_t, transport_t);
bool adjlist_contains (map_adj *, location_t, transport_t);

#endif // !defined(FOD__MAP_H_)
