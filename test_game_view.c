////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"
#include "queue.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score (gv) == GAME_START_SCORE);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 1);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (gv_get_location (gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",  "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_location (gv, PLAYER_DRACULA) == GENEVA);
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5);
		assert (gv_get_health (gv, PLAYER_DRACULA) == 30);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Dracula View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for connections");
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		do {
			puts ("Checking Galatz road connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				GALATZ, PLAYER_LORD_GODALMING, 0,
				true, false, false
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 5);
			assert (seen[GALATZ]);
			assert (seen[CONSTANTA]);
			assert (seen[BUCHAREST]);
			assert (seen[KLAUSENBURG]);
			assert (seen[CASTLE_DRACULA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Ionian Sea sea connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 7);
			assert (seen[IONIAN_SEA]);
			assert (seen[BLACK_SEA]);
			assert (seen[ADRIATIC_SEA]);
			assert (seen[TYRRHENIAN_SEA]);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);
			assert (seen[SALONICA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Athens rail connections (none)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false
			);

			assert (n_edges == 1);
			assert (edges[0] == ATHENS);

			free (edges);
		} while (0);

		puts ("passed");
		gv_drop (gv);
	} while (0);

	// test for 
	return EXIT_SUCCESS;
}

void white_box_testing(GameView new) {
	puts("test1: checking fill_in_hunter");
	// checking that location_to_id is functioning correctly
		// checking for a known location
		// checkig for uknown sea
		// checking for uknown location
	// check that dracula and hunter[] structs are filled correctly 
		// are values as expected
	// check that past plays is filled correctly after one round (struct_to_pastPlays)
		// is it empty on rund 0
		// at the start of round > 0 does pastPlays[round-1] = current structs for hunter and dracula

	// chekcing fill_in_hunter
	// does hunter_encounters change corresponding values and arrays
		// T
			// is trap_locations array updated via remove_malfunction_trap
			// is health decremented
			// does health ever go below zero
		// V
			// is vampire location set to nowhere
		// D
			// is health decrmented for both dracula and hunter
			// does health never go below 0
		// .
			// do all struct variables for the current hunter and dracula stay the same

	//checkig is hunter on draculas trail
		// if hunter isnt on draculas trail the pblic dracula trail should be empty (-1)
		// if hunter has stumbled upon draculas trail then the public trail should show
		// locations dracula travelled to to get to the hunters location

	// checking was hunter resting
		// checking that health is increased if hunter was resting
			// checking that health never goes above 9
		// cehcking that it is the same if the hunter was not
		// cehcking if other hunters were resting
			// testing for if hunter is MINA or GODALMING since both trigger different conditions
				// checking that public dracula trail matches draculas trail if all huhnters were resting
				//checking that public dracula trail remains unchanged if not all hunters were resting


	// checking that after a hunter is reuced to zero health, next round they have 9 health
		// checking that the score decreases correctly
	
	// ----

	// checking fill_in_dracula is functioning correctly
		// checking that draculas trail is updated correctly/	
			// is nfilled incremented
			// is nfilled never above 6
			// is the new location at the top
			// is the previous location right below

	// checking is_dracula_at_sea works correctly
		// checking that if dracula is at sea that health point get taken away 
		// checking that if dracula doubled back to sea(1-5) then he gets health taken off
		// chdck that it works for unknown sea and know sea locations
		
	// checking is_dracula_at_his_castle
		// if he is, does health get increased correctly
			// chekcing health never rises above full (40)
		// is_dracula_at_castle is set to true?
		// if he isnt then is_dracula_at_castle set to false
	
	// cehck_draculas_encounters
		// T
			// check that the corresponding location is added to the trap_locations array
		// V
			// check that vampire_location is set to the corresponding location
		// M
			// check that a trap has malfunctioned and its corresponding location has been taken out of the array (i.e the very first index
		// V(2)
			// check that the score is decremented by the corresponding value
}
