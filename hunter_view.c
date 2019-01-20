////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
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
#include "hunter_view.h"
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
typedef struct hunter_view {
	game_view *game;
} hunter_view;

hunter_view *hv_new (char *past_plays, player_message messages[])
{
	hunter_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate HunterView");
	new->game = gv_new(past_plays, messages);
	
	return new;
}

void hv_drop (hunter_view *hv)
{
	gv_drop(hv->game);
	free (hv);
}

round_t hv_get_round (hunter_view *hv)
{
	return gv_get_round(hv->game);;
}

enum player hv_get_player (hunter_view *hv)
{
	return gv_get_player(hv->game);
}

int hv_get_score (hunter_view *hv)
{
	return gv_get_score(hv->game);
}

int hv_get_health (hunter_view *hv, enum player player)
{
	return gv_get_health(hv->game, player);
}

location_t hv_get_location (hunter_view *hv, enum player player)
{
	return gv_get_location(hv->game, player);
}

void hv_get_trail (
	hunter_view *hv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	gv_get_history(hv->game, player, trail);
}

location_t *hv_get_dests (
	hunter_view *hv, size_t *n_locations,
	bool road, bool rail, bool sea)
{
	*n_locations = 0;
	location_t *conn = gv_get_connections(hv->game, n_locations, gv_get_location(hv->game, gv_get_player(hv->game)), gv_get_player(hv->game), gv_get_round(hv->game), road, rail, sea);
	return conn;
}

location_t *hv_get_dests_player (
	hunter_view *hv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	location_t *conn = gv_get_connections(hv->game, n_locations, gv_get_location(hv->game, player), player, gv_get_round(hv->game), road, rail, sea);
	return conn;
}
