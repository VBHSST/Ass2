////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "dracula_view.h"
#include "game.h"
#include "game_view.h"
#include "cycle.h"
#include "map.h" 

typedef struct cycle cycle;
typedef struct link link;

typedef struct play {
	location_t location; 
	int health;
	char encounter[4]; // max 4 for hunters
	//char message[MESSAGE_SIZE]; // max 100
} play;

typedef struct public_dracula_trail {
	location_t location;
	int round;
} p_dracula_trail;

typedef struct game_view {
	// round number
	round_t round; 
	// point score;
	int score;
	// player turn 
	enum player turn;

	location_t vampire_location;

	location_t trap_locations[6]; 

	cycle *dracula_trail;
	p_dracula_trail public_dracula_trail[6];
	bool is_dracula_at_castle;

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

typedef struct dracula_view {
	location_t trap_locations[TRAIL_SIZE];
	location_t vamp_location;

	// Hunters Structs
	struct hunt {
		location_t location;
		int health;
	} hunter[4];

	// Dracula struct
	struct drac {
		location_t location;
		int health;
	} dracula;

	game_view *game;
} dracula_view;

//SHARED
static location_t char_to_locationID (char *past_plays, int index);
static void remove_malfunctioned_trap (dracula_view *new, location_t location);
//DRACULA
static void fill_in_dracula(char *past_plays, int index, dracula_view *new);
	static void check_draculas_encounters(dracula_view *new, char *past_plays, int index);
//HUNTER
static void fill_in_hunter(char *past_plays, int index, enum player player, dracula_view *new);
	static void check_hunter_encounters(dracula_view *new, char *past_plays, enum player player, int index);

dracula_view *dv_new (char *past_plays, player_message messages[])
{
	dracula_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate DraculaView");

	// set up gameview with past plays
	new->game = gv_new(past_plays, messages);
	for (int i = 0; i < TRAIL_SIZE; i++) {
		new->trap_locations[i] = NOWHERE;
	}
	new->vamp_location = NOWHERE;

	for (int i = 0; past_plays[i] != '\0' && i < (GAME_START_SCORE * NUM_PLAYERS); i+=7) {
		if (i != 0) {
			i++;
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
				break;
		}
	}
	return new;
}

void dv_drop (dracula_view *dv)
{
	gv_drop(dv->game);
	free (dv);
}

round_t dv_get_round (dracula_view *dv)
{
	return gv_get_round(dv->game);
}

int dv_get_score (dracula_view *dv)
{
	return gv_get_score(dv->game);
}

int dv_get_health (dracula_view *dv, enum player player)
{
	return gv_get_health(dv->game, player);
}

location_t dv_get_location (dracula_view *dv, enum player player)
{
	return gv_get_location(dv->game, player);
}

void dv_get_player_move (
	dracula_view *dv, enum player player,
	location_t *start, location_t *end)
{
	// find player
	location_t *ptrail = malloc(sizeof(location_t) * 6);
	//get their trail from gv
	gv_get_history(dv->game, player, ptrail);
	//return 0, 1 index of for start, end
	*start = ptrail[1];
	*end = ptrail[0];
	
	return;
}

void dv_get_locale_info (
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	*n_traps = 0;
	*n_vamps = 0;
	// iterate over trap location to find matches with 'where'
	for (int i = 0; i < TRAIL_SIZE; i++) {
		if (where == dv->trap_locations[i]) {
			(*n_traps)++;
		}
	}
	if (where == dv->vamp_location) {
		*n_vamps = 1;
	} 
	
	return;
}

void dv_get_trail (
	dracula_view *dv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	gv_get_history(dv->game, player, trail);
}

location_t *dv_get_dests (
	dracula_view *dv, size_t *n_locations, bool road, bool sea)
{
	int rail = false;
	*n_locations = 0;
	location_t *conn = gv_get_connections(dv->game, n_locations, gv_get_location(dv->game, PLAYER_DRACULA), PLAYER_DRACULA, gv_get_round(dv->game), road, rail, sea);
	return conn;
}

location_t *dv_get_dests_player (
	dracula_view *dv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	*n_locations = 0;
	location_t *conn = gv_get_connections(dv->game, n_locations, gv_get_location(dv->game, player), player, gv_get_round(dv->game), road, rail, sea);
	return conn;
}

//STATIC FUNCTIONS
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

static void remove_malfunctioned_trap (dracula_view *new, location_t location) {
	for (int k = 0; k < 6; k++) {
		if (new->trap_locations[k] == location) {
			new->trap_locations[k] = NOWHERE;
			break;
		}
	}
}

static void fill_in_hunter(char *past_plays, int index, enum player player, dracula_view *new) {
	new->hunter[player].location = char_to_locationID(past_plays, index);

	index+=2;
	check_hunter_encounters(new, past_plays, player, index);

	
} 

static void check_hunter_encounters(dracula_view *new, char *past_plays, enum player player, int index) {
	for (int i = index, j = 0; past_plays[i] != ' ' && past_plays[i] != '\0'; i++, j++) {
		switch (past_plays[i]) {
			case 'T': 
				remove_malfunctioned_trap (new, new->hunter[player].location);
				break;
			case 'D':
				break;
			case 'V':
				new->vamp_location = NOWHERE;
				break;
			default:
				break;	
		}
	}
}

static void fill_in_dracula(char *past_plays, int index, dracula_view *new) {
	new->dracula.location = char_to_locationID(past_plays, index);
	check_draculas_encounters(new, past_plays, index);
	
}

static void check_draculas_encounters(dracula_view *new, char *past_plays, int index) {
 	if (past_plays[index+=2] == 'T') {	
		for (int k = 0; k < 6; k++) {
			if (new->trap_locations[k] == NOWHERE) {
				new->trap_locations[k] = new->dracula.location;
				break;
			}
		}
	} 
	if (past_plays[index+=1] == 'V') {
		new->vamp_location = new->dracula.location;
	} 
	if (past_plays[index+=1] == 'M') {
		remove_malfunctioned_trap (new, new->dracula.location);
	} else if (past_plays[index] == 'V') {
		new->vamp_location = NOWHERE;
	} else {
	}
}
