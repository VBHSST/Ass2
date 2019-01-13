////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "game.h"
#include "game_view.h"
#include "map.h" //... if you decide to use the Map ADT

// array for each players previous turn
typedef struct player {
	location_t location[366]; //current location and location they decided to move to
	size_t health;
	char encounter[4]; // max 4 for hunters
	char action; 
	char message[99]; // max 99
} player;

typedef struct game_view {
	// round number
	round_t round; 
	// point score;
	size_t score;
	// player turn 
	enum player turn;
	location_t trap_location[6];
	location_t vampire_location;

	struct player pastPlays[366][5]; // -1 point every round, 5 turns per round

	// Hunters Structs
	struct hunter {
		location_t location;
		size_t health;
	} hunters[4];

	// Dracula struct
	struct dracula {
		//TRAIL
		location_t location;
		size_t health;
	} dracula;

	//Map map; // need to figure out how to initialize this map in this file

} game_view;


// prototypes
static location_t new_from(location_t location[], int i);
static bool loc_unknown (location_t loc);
static int num_conn(Map g, LocationID start, PlayerID player, Round round, bool road, bool rail, bool sea);
static void BFS(Map map, location_t from, int times, location_t conn[], int i);
static bool exist (location_t curr, location_t trail[], int index);
static void array_conn() (Map map, location_t from, location_t conn[], enum player player, round_t round, bool road, bool rail, bool sea);

game_view *gv_new (char *past_plays, player_message messages[])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
	// intialize starting values for all variables
		// round  = 0;
		// score  = 366
		// turn = 0 or Lord goldaming enum 

		//hunter
			// location = NOWHERE
			// health = GAME_START_HUNTER_LIFE_POINTS
			// message = NULL;
		// Dracula
			// location = NOWHERE
			// health = GAME_START_BLOOD_POINTS
		
	// read through past plays
		// store location, encounter and/or action in relative player 
		// this is done by using turn num mod 5 

		// if there is an encounter apply necessary change 
			// if dracula
					// set it down into the traps array
					// if array full take out the oldest trap and remove it from the 
				// places vampire
					// set vampires location
			// else (hunter)
				// trap
					// take away 2 health
					// remove one trap from location
				// immature vampire
				// dracula
		// if there is a location effect for dracula apply it
			// SEA
			// at Castle
		// if dracula has an action
			// if trap
				// remove it from the front of the trap lcoation array
			// if vampire
				// remove form vamppire location
				// remove 16 points from the score
				

		// if hunters are in the same city as last turn
			// restore health

		// if a vampire was set more than 6 turns ago and wasnt vanquished

		// if draculas health is 0
			// end game
		// if hunters heath is 0 on the last round
			// current round will start at hospital


	// TODO: EVE - initialize map
		// create new map
		// add connections


	// UNNECESSARY
		// each turn check the vampires turn 6 rounds ago (6 rounds back in past turns)
			// if it was a trap
				// remove it from the map and from traps array
			// if it was a vampire
				// remove it from the map
				// take away 13 points from score
	return new;
}
// NOTES: 
// - need to figure out best way to maintain vampires trail (so that 
// vanquished vampires are accounted for and grown vampires as well
void gv_drop (game_view *gv)
{
	// TODO: EVE - complete
	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	return gv->round;
}

enum player gv_get_player (game_view *gv)
{
	return gv->turn;
}

int gv_get_score (game_view *gv)
{
	return gv->score;
}

int gv_get_health (game_view *gv, enum player player)
{
	if (player != PLAYER_DRACULA)
		return gv->hunters[player]->health;
	else 
		return gv->dracula->health;
}

location_t gv_get_location (game_view *gv, enum player player)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *gv_get_connections (
	game_view *gv, size_t *n_locations,
	location_t from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	if (gv == NULL) return NULL;

	// if player is out of range
	if (player < 0 || player > 4) return NULL;
	if (round > gv_get_round(gv)) return NULL;

	Map map = map_new();

	// if the place is unknown
	if (from < Adriatic_Sea || from > ZURICH) {
		// update the place, starts from most recent one (ZERO)
		from = new_from(gv->pastPlays[round][player].location, 0);
	}

	*n_locations = num_conn(map, from, player, round, road, rail, sea);
	location_t *conn = malloc(sizeof(location_t) * (*n_locations));

	for (int i = 0; i < (*n_locations); i++) {
		conn[i] = NOWHERE;
	}

	array_conn(map, from, conn, player, round, road, rail, sea);
	map_drop(map);

	return conn;
}


// help functions

// Give the new location if the current one is out of range
static location_t new_from(location_t location[], int i) {
	location_t temp = location[i];

	// if the location is unknown, make a new one according to cases
	if (temp < Adriatic_Sea || temp > ZURICH) {
		switch (temp){
			case HIDE:
				if(! loc_unknown(location[i+1])) 
					return new_from(location, i+1);
			case DOUBLE_BACK_1:                
				if(! loc_unknown(location[i+1]))
                    return new_from(location, i+1);
			case DOUBLE_BACK_2:
 				if(! loc_unknown(location[i+2]))
                    return new_from(location, i+2);				                
			case DOUBLE_BACK_3:
			    if(! loc_unknown(location[i+3]))
                    return new_from(location, i+3);
			case DOUBLE_BACK_4:
			    if(! loc_unknown(location[i+4]))
                    return new_from(location, i+4);
			case DOUBLE_BACK_5:
			    if(! loc_unknown(location[i+5]))
                    return new_from(location, i+5);
			case TELEPORT: return CASTLE_DRACULA; 			
		}
	}

	return temp;
}

// return 1 if the location is unknown
static bool loc_unknown (location_t loc) {
    return (! (loc != CITY_UNKNOWN 
	    	&& loc != SEA_UNKNOWN 
	    	&& loc != UNKNOWN_LOCATION 
	    	&& loc != NOWHERE));
}

// Count direct connections between two locations
static int num_conn(Map map, location_t from, 
	enum player player, round_t round, 
	bool road, bool rail, bool sea) {

	assert(map != NULL);

	int i = 0;
	if (from < Adriatic_Sea || from > ZURICH) {
		return 0;
	}

	location_t conn[NUM_MAP_LOCATIONS];

	for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
		conn[i] = NOWHERE;
	}

	conn[i++] = from;

	// DRACULA
	if (player == PLAYER_DRACULA) {
        for(map_adj *curr = map->connections[from]; curr != NULL; curr = curr->next) {
            if (road == true && curr->type == ROAD && curr->v != ST_JOSEPH_AND_ST_MARYS) {
                conn[i++] = curr->v;
            }
            if (sea == true && curr->type == BOAT) {
                conn[i++] = curr->v;
                for (map_adj *curr2 = map->connections[conn[i-1]]; curr2!=NULL; curr2 = curr2->next) {
                    if (curr2->type == BOAT) {
                        conn[i] = curr2->v;
                    }
                }
            }
        }
    // hunter   
    } else {
        for (map_adj *curr = map->connections[from]; curr != NULL; curr = curr->next) {
            int rails = (round + player) % 4; 
            if (rails == 0) rail = false;
            if (road == true && curr->type == ROAD) {
                conn[i++] = curr->v;
            }
            if (rail == true && curr->type == RAIL) {
                BFS(map, curr->v, rails, conn, i);
            }
            if (sea == true && 
            	curr->type == BOAT && location_get_type(from) != SEA) {

                conn[i++] = curr->v;
                for (map_adj *curr2 = map->connections[conn[i-1]]; curr2 != NULL; curr2 = curr2->next) {
                    if (curr2->type == BOAT) {
                        conn[i++] = curr2->v;
                    }
                }

            } else if (sea == true && location_get_type(from) == SEA) {
                conn[i++] = curr->v;
            }
        }
    }
    return i;
}

static void BFS(Map map, location_t from, int times, location_t conn[], int i) {
    Queue q = newQueue();
    QueueJoin(q, from);
    
    Queue hop = newQueue();
    QueueJoin(q, times);
    
    while (!QueueIsEmpty(q)) {
        from = QueueLeave(q);
        times = QueueLeave(hop);
        if (times == 0 || exist(from, conn, i))
            continue;
        conn[i++] = from;
        for (map_adj *curr = map->connections[from]; curr != NULL; curr = curr->next) {
            if (curr->type == RAIL) {
                QueueJoin(q, curr->v);
                QueueJoin(q, times-1);
            
            }
        }
    }
    dropQueue(q);
    dropQueue(hop);
}

// check whether a location is already in the trail
static bool exist (location_t curr, location_t trail[], int index) {
    for (int i = 0; i < index; i++) {
        if (curr == trail[index]) return TRUE;
    }
    return FALSE;
}

static void array_conn() (Map map, location_t from, 
	location_t conn[], enum player player, round_t round, 
	bool road, bool rail, bool sea) {
    
	assert(map != NULL);

	int i = 0;
	if (from < Adriatic_Sea || from > ZURICH) {
		return;
	}

	conn[i++] = from;

	// DRACULA
	if (player == PLAYER_DRACULA) {
        for(map_adj *curr = map->connections[from]; curr != NULL; curr = curr->next) {
            if (road == true &&
            curr->type == ROAD && 
            curr->v != ST_JOSEPH_AND_ST_MARYS) {
                conn[i++] = curr->v;
            }
            if (sea == true && curr->type == BOAT) {
                conn[i++] = curr->v;
                for (map_adj *curr2 = map->connections[conn[i-1]]; curr2!=NULL; curr2 = curr2->next) {
                    if (curr2->type == BOAT) {
                        conn[i] = curr2->v;
                    }
                }
            }
        }
    // hunter   
    } else {
        for (map_adj *curr = map->connections[from]; curr != NULL; curr = curr->next) {
            int rails = (round + player) % 4; 
            if (rails == 0) rail = false;
            if (road == true && curr->type == ROAD) {
                conn[i++] = curr->v;
            }
            if (rail == true && curr->type == RAIL) {
                BFS(map, curr->v, rails, conn, i);
            }
            if (sea == true && 
            	curr->type == BOAT && 
            	location_get_type(from) != SEA) {

                conn[i++] = curr->v;
                for (map_adj *curr2 = map->connections[conn[i-1]]; curr2 != NULL; curr2 = curr2->next) {
                    if (curr2->type == BOAT) {
                        conn[i++] = curr2->v;
                    }
                }

            } else if (sea == true && location_get_type(from) == SEA) {
                conn[i++] = curr->v;
            }
        }
    }
}
