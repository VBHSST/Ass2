########################################################################
# COMP2521 19t0 // The Fury of Dracula // the View
# view/Makefile: build tests for {Game,Hunter,Dracula}View
#
# 2018-12-31	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
# notes: added queue.c and map.c to game_view.o compiler

CC	 = 2521 3c
BINS	 = test_game_view test_hunter_view test_dracula_view

all: ${BINS}

test_game_view:		test_game_view.o game_view.o map.o places.o queue.o
test_game_view.o:	test_game_view.c game_view.h map.h places.h game.h queue.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o map.o places.o queue.o
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h map.h places.h game.h queue.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o map.o places.o queue.o
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h map.h places.h game.h queue.h

queue.o:	queue.c queue.h places.h
places.o:	places.c places.h game.h
map.o:		map.c map.h places.h game.h
game_view.o:	game_view.c queue.c map.c game_view.h game.h map.h queue.h
hunter_view.o:	hunter_view.c hunter_view.h game.h
dracula_view.o:	dracula_view.c dracula_view.h game.h

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core

.PHONY: prove
prove: test_game_view test_hunter_view test_dracula_view
	prove -f ./test_game_view ./test_hunter_view ./test_dracula_view
