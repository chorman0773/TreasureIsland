/*
 * Main.c
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <internal/Game.h>
#include <tigame/Random.h>

int main(){
	Random* rand = Random_new();
	Game* game = tigame_Game_allocateCOMStructure();
	Random_seed(rand);


	tigame_Game_cleanup(game);
	Random_free(rand);
}
