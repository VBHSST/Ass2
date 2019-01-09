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
		char message[99];
	} hunters[4];

	// Dracula struct
	struct dracula {
		//TRAIL
		location_t location;
		size_t health;
	} dracula;

	Map map; // need to figure out how to initialize this map in this file

} game_view;


// array for each players previous turn
typedef struct player {
	location_t location; //current location and location they decided to move to
	char encounter[4]; // max 4 for hunters
	char action; 
	char message[99]; // max 99
} player;


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
				

		// if hunters are in the smae city as last turn
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
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

enum player gv_get_player (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int gv_get_score (game_view *gv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int gv_get_health (game_view *gv, enum player player)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
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
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}
