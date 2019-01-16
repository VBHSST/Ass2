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
#include <stdio.h>

#include "game.h"
#include "game_view.h"
#include "map.h" 
#include "queue.h"

#define TRUE 1
#define FALSE 0

typedef struct queue queue;
typedef struct queue_node queue_node;

typedef struct play {
	location_t location; 
	int health;
	char encounter[4]; // max 4 for hunters
	//char message[MESSAGE_SIZE]; // max 100
} play;

typedef struct public_dracula_trail {
	location_t location;
	int round;
} public_dracula_trail;

typedef struct game_view {
	// round number
	round_t round; 
	// point score;
	int score;
	// player turn 
	enum player turn;

	location_t vampire_location;

	location_t trap_locations[6]; 

	queue *dracula_trail;
	public_dracula_trail public_dracula_trail[6];

	play pastPlays[GAME_START_SCORE][NUM_PLAYERS]; 

	// Hunters Structs
	struct hunter {
		location_t location;
		int health;
		char encounter[4];
		//char message[MESSAGE_SIZE];
	} hunter[4];

	// Dracula struct
	struct dracula {
		location_t location;
		int health;
		char encounter[4];
	} dracula;

	//Map map; // need to figure out how to initialize this map in this file

} game_view;

static void structs_to_pastPlays(game_view *new);


//shared functions
static location_t char_to_locationID (char *past_plays, int index);
static void remove_malfunctioned_trap (game_view *new, location_t location);

//hunter functions
static void fill_in_hunter(char *past_plays, int index, enum player player, game_view *new);
	static void was_hunter_resting(game_view *new, enum player player);
	static void check_hunter_encounters(game_view *new, char *past_plays, enum player player, int index);

//dracula functions
static void fill_in_dracula(char *past_plays, int index, game_view *new);
	static void enqueue_draculas_new_location_to_trail(game_view *new);
	static void is_dracula_at_sea (game_view *new);
	static void is_dracula_at_his_castle(game_view *new);
	static void check_draculas_encounters(game_view *new, char *past_plays, int index);

// map functions
static location_t new_from(location_t location[], int i);
static bool loc_unknown (location_t loc);
static int num_conn(Map g, LocationID start, PlayerID player, Round round, bool road, bool rail, bool sea);
static void BFS(Map map, location_t from, int times, location_t conn[], int i);
static bool exist (location_t curr, location_t trail[], int index);
static void array_conn() (Map map, location_t from, location_t conn[], enum player player, round_t round, bool road, bool rail, bool sea);


game_view *gv_new (char *past_plays, player_message messages[])
{
	game_view *new = malloc (sizeof (struct game_view));
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
	
	// intialize starting values for all variables

	// GENERAL
	new->round = 0;
	new->score = GAME_START_SCORE; //366
	new->turn = PLAYER_LORD_GODALMING;  //0 

	new->vampire_location = NOWHERE;

	for(int i = 0; i < TRAIL_SIZE; i++) {
		new->trap_locations[i] = NOWHERE; 
	}

	new->dracula_trail = queue_new();

	for (int i = 0; i < 6; i++) {
		new->public_dracula_trail[i].location = NOWHERE;
		new->public_dracula_trail[i].round = -1;  
	}

	//HUNTER
	for (int i = 0; i < 4; i++) {
		new->hunter[i].location = NOWHERE;
		new->hunter[i].health = GAME_START_HUNTER_LIFE_POINTS;
		//new.hunter[i].message = {""};
	}

	// DRACULA
	new->dracula.location = NOWHERE;
	new->dracula.health = GAME_START_BLOOD_POINTS;	
		
	// read through past_plays
	for (int i = 0; past_plays[i] != '\0' && i < (GAME_START_SCORE * NUM_PLAYERS); i+=7) {
		if (i != 0) {
			i++;
		}
		printf("turn = %d\n", new->turn);

		if (i % 40 == 0 && i != 0) {
			structs_to_pastPlays(new);	
		}
		
		switch (past_plays[i]) {
			case 'G': 
				fill_in_hunter(past_plays, i+1, PLAYER_LORD_GODALMING, new);
				break;
			case 'S': 
				fill_in_hunter(past_plays, i+1, PLAYER_DR_SEWARD, new);
				break;
			case 'H': 
				fill_in_hunter(past_plays, i+1, PLAYER_VAN_HELSING, new);
				break;
			case 'M': 
				fill_in_hunter(past_plays, i+1, PLAYER_MINA_HARKER, new);
				break;
			default: 
				fill_in_dracula(past_plays, i+1, new);
				new->round++;
				break;
		}
		

	new->score--;
	new->turn++;
	new->turn = new->turn % 5;
	}

	return new;
}

static void structs_to_pastPlays(game_view *new) {
	// HUNTERS
	for (int j = 0; j < 4; j++) {
		new->pastPlays[new->round -1][j].location = new->hunter[j].location;
		new->pastPlays[new->round -1][j].health = new->hunter[j].health;
		for (int k = 0; k < 4; k++) {
			new->pastPlays[new->round-1][j].encounter[k] = new->hunter[j].encounter[k];
		}
		// add message implementation here
	}
	// DRACULA
	new->pastPlays[new->round-1][4].location = new->dracula.location;
	new->pastPlays[new->round-1][4].health = new->dracula.health;
	for (int k = 0; k < 4; k++) {
		new->pastPlays[new->round-1][4].encounter[k] = new->dracula.encounter[k];
	}
	// add message implementation here
}

static location_t char_to_locationID (char *past_plays, int index) {
	char loc[2];
	loc[0] = past_plays[index];
	loc[1] = past_plays[index+1];

	if (loc[0] == 'C' && loc[1] == '?')			return CITY_UNKNOWN;
	else if (loc[0] == 'S' && loc[1] == '?')	return SEA_UNKNOWN;
	else if (loc[0] == 'H' && loc[1] == 'I')	return HIDE;
	else if (loc[0] == 'D' && loc[1] == '1')	return DOUBLE_BACK_1;
	else if (loc[0] == 'D' && loc[1] == '2')	return DOUBLE_BACK_2;
	else if (loc[0] == 'D' && loc[1] == '3')	return DOUBLE_BACK_3;
	else if (loc[0] == 'D' && loc[1] == '4')	return DOUBLE_BACK_4;
	else if (loc[0] == 'D' && loc[1] == '5')	return DOUBLE_BACK_5;
	else if (loc[0] == 'T' && loc[1] == 'P')	return TELEPORT;
	else if (loc[0] == '?' && loc[1] == '?') 	return UNKNOWN_LOCATION;
	else {
		location_t location = location_find_by_abbrev(loc);
		return location;
	}
}


static void fill_in_hunter(char *past_plays, int index, enum player player, game_view *new) {

	// find and set location
	new->hunter[player].location = char_to_locationID(past_plays, index);
	
	// check the hunters encounters
	index+=2;
	check_hunter_encounters(new, past_plays, player, index);

	// check if the hunter died this round
	if (new->hunter[player].health == 0) new->score -= SCORE_LOSS_HUNTER_HOSPITAL;
	
	// check if the hunter was resting
	was_hunter_resting(new, player);
	
} 

static void check_hunter_encounters(game_view *new, char *past_plays, enum player player, int index) {
	puts("start");
	// check for trap on location
	for (int i = index; past_plays[i] != ' ' && past_plays[i] != '\0'; i++) {
		printf("char: '%c'\n", past_plays[i]);
		switch (past_plays[i]) {
			case 'T': 
				new->hunter[player].health -= LIFE_LOSS_TRAP_ENCOUNTER;	
				remove_malfunctioned_trap (new, new->hunter[player].location);
				break;
			case 'D':
				new->hunter[player].health -= LIFE_LOSS_DRACULA_ENCOUNTER;
				new->dracula.health -= LIFE_LOSS_HUNTER_ENCOUNTER;
				break;
			case 'V':
				new->vampire_location = NOWHERE;
				break;
			default:
				break;	
		}
	}
}

static void was_hunter_resting(game_view *new, enum player player) {
	
	if (new->round > 0 && new->pastPlays[new->round -1][player].location == new->hunter[player].location) {
		new->hunter[player].health += LIFE_GAIN_REST;
		if (new->hunter[player].health > GAME_START_HUNTER_LIFE_POINTS) {
			new->hunter[player].health = GAME_START_HUNTER_LIFE_POINTS;
		}
	}	
}

static void remove_malfunctioned_trap (game_view *new, location_t location) {
	for (int k = 0; k < 6; k++) {
		// remove trap from active traps
		if (new->trap_locations[k] == location) {
			new->trap_locations[k] = NOWHERE;
			break;
		}
	}
}

static void fill_in_dracula(char *past_plays, int index, game_view *new) {

	new->dracula.location = char_to_locationID(past_plays, index);

	// add draculas current location to trail
	enqueue_draculas_new_location_to_trail(new);

	// check if dracula is at sea
	is_dracula_at_sea(new);

	// check if dracula is at his castle
	is_dracula_at_his_castle(new);

	// check for draculas encounters
	check_draculas_encounters(new, past_plays, index);
	
}

static void enqueue_draculas_new_location_to_trail(game_view *new) {
	if (new->dracula_trail->n_locations == new->dracula_trail->max_locations) {
		location_t removed = queue_de(new->dracula_trail);
	}
	queue_en(new->dracula_trail, new->dracula.location);
}

static void is_dracula_at_sea (game_view *new) {
	puts("1");

	location_t *dtrail = malloc(sizeof(location_t) * 6);
	gv_get_history(new, PLAYER_DRACULA, dtrail);

	switch (new->dracula.location) {
		case (DOUBLE_BACK_1): 
			puts("1.1");
			if (dtrail[1] != UNKNOWN_LOCATION && dtrail[1] != CITY_UNKNOWN && dtrail[1] != SEA_UNKNOWN) {
				printf("%d\n", dtrail[1]);
				if (location_get_type(dtrail[1]) == SEA) {
					puts("1.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (dtrail[1] == SEA_UNKNOWN) {
					puts("1.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;
		case (DOUBLE_BACK_2):
			puts("2.1");
			if (dtrail[2] != UNKNOWN_LOCATION && dtrail[2] != CITY_UNKNOWN && dtrail[2] != SEA_UNKNOWN) {
				printf("%d\n", dtrail[2]);
				if (location_get_type(dtrail[2]) == SEA) {
					puts("2.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (dtrail[2] == SEA_UNKNOWN) {
					puts("2.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;

		case (DOUBLE_BACK_3):
			puts("3.1");
			if (dtrail[3] != UNKNOWN_LOCATION && dtrail[3] != CITY_UNKNOWN && dtrail[3] != SEA_UNKNOWN) {
				printf("%d\n", dtrail[3]);
				if (location_get_type(dtrail[3]) == SEA) {
					puts("3.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (dtrail[3] == SEA_UNKNOWN) {
					puts("3.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;	
		case (DOUBLE_BACK_4):
			puts("4.1");
			if (dtrail[4] != UNKNOWN_LOCATION && dtrail[4] != CITY_UNKNOWN && dtrail[4] != SEA_UNKNOWN) {
				printf("%d\n", dtrail[4]);
				if (location_get_type(dtrail[4]) == SEA) {
					puts("4.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (dtrail[4] == SEA_UNKNOWN) {
					puts("4.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;

		case (DOUBLE_BACK_5):
			puts("5.1");
			if (dtrail[5] != UNKNOWN_LOCATION && dtrail[5] != CITY_UNKNOWN && dtrail[5] != SEA_UNKNOWN) {
				printf("%d\n", dtrail[5]);
				if (location_get_type(dtrail[5]) == SEA) {
					puts("5.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (dtrail[5] == SEA_UNKNOWN) {
					puts("5.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;
		
		default:
			puts("6.1");
			if ((new->dracula.location != UNKNOWN_LOCATION) && 
				(new->dracula.location != CITY_UNKNOWN) && 
				(new->dracula.location != SEA_UNKNOWN)) {

				printf("%d\n", new->dracula.location);
				if (location_get_type(new->dracula.location) == SEA) {
					puts("6.2");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			} else {
				if (new->dracula.location == SEA_UNKNOWN) {
					puts("6.3");
					new->dracula.health -= LIFE_LOSS_SEA;
				}
			}
			break;
	}
}

static void is_dracula_at_his_castle(game_view *new) {
	if (new->dracula.location == CASTLE_DRACULA) {
		new->dracula.health+=LIFE_GAIN_CASTLE_DRACULA;

		if (new->dracula.health > GAME_START_BLOOD_POINTS) {
			new->dracula.health = GAME_START_BLOOD_POINTS;
		}
	}
}

static void check_draculas_encounters(game_view *new, char *past_plays, int index) {
 	if (past_plays[index+=2] == 'T') {	
		for (int k = 0; k < 6; k++) {
			if (new->trap_locations[k] == NOWHERE) {
				new->trap_locations[k] = new->dracula.location;
				break;
			}
		}
	} 
	if (past_plays[index+=1] == 'V') {
		new->vampire_location = new->dracula.location;
	} 
	if (past_plays[index+=1] == 'M') {
		remove_malfunctioned_trap (new, new->dracula.location);
	} else if (past_plays[index] == 'V') {
		new->score -= SCORE_LOSS_VAMPIRE_MATURES;
		new->vampire_location = NOWHERE;
	} else {
	}
}

void gv_drop (game_view *gv)
{
	queue_drop(gv->dracula_trail);
	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	assert(gv != NULL);
	return gv->round;
}

enum player gv_get_player (game_view *gv)
{
	assert(gv != NULL);
	return gv->turn;
}

int gv_get_score (game_view *gv)
{
	assert(gv != NULL);
	return gv->score;
}

int gv_get_health (game_view *gv, enum player player)
{
	assert(gv != NULL);
	assert((player >= PLAYER_LORD_GODALMING) && 
			(player <= PLAYER_DRACULA));
	if (player == PLAYER_DRACULA) {
		return gv->dracula.health;
	} else {
		printf("hunter: %d, health: %d\n", player, gv->hunter[player].health);
		return gv->hunter[player].health;
	}
}

location_t gv_get_location (game_view *gv, enum player player)
{
	assert(gv != NULL);
	assert((player >= PLAYER_LORD_GODALMING) && 
			(player <= PLAYER_DRACULA));
	if (player == PLAYER_DRACULA) {
		return gv->dracula.location;
	} else {
		return gv->hunter[player].location;
	}
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	assert(gv != NULL);
	assert((player >= PLAYER_LORD_GODALMING) && 
			(player <= PLAYER_DRACULA));

	for (int j = 0; j < TRAIL_SIZE; j++) {
		trail[j] = UNKNOWN_LOCATION; 
	}	

	int i = 0;
	int trailSize = 0;
	
	if (gv->turn > player) {
		trail[0] = gv->hunter[player].location;
		i++;
		trailSize++;

		if (gv->round > 0) {
			for (int j = i, k = trailSize; (gv->round -j) >= 0 && k < 6; j++, k++) {
				trail[j] = gv->pastPlays[gv->round - i][player].location;
				puts("plsu1");
			}
			for (int k = 0; k < 6; k++) {
				printf("trail[%d] = %s\n", k, location_get_name(trail[k]));
			}
		}
		
	} else {
		i = 0;
		if (player == PLAYER_DRACULA) {
			i = 0;
			for (queue_node *curr = gv->dracula_trail->head; curr != NULL; curr = curr->next) {
				trail[gv->dracula_trail->n_locations -1-i] = curr->location;
				i++;
			}
		} else {
			if (gv->round > 0) {
				for (int j = i, k = trailSize; (gv->round -1 -j) >= 0 && k < 6; j++, k++) {
					trail[j] = gv->pastPlays[gv->round -1 - i][player].location;
				}
			}
		}
	}	
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
        if (times == 0 || 
        	exist(from, conn, i))
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

static void array_conn (Map map, location_t from, 
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
