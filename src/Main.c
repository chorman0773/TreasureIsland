/*
 * Main.c
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <internal/Game.h>
#include <internal/Random.h>

int main(){
	Random* rand = Random_new();
	Game* game = tigame_Game_allocateCOMStructure();
	Random_seed(rand);
	(*game)->printf(game,"Treasure Island Game: Initialized with Version %hd\n",(*game)->getVersion(game));

	(*game)->printf(game,"Goodbye\n");
	tigame_Game_cleanup(game);
	Random_free(rand);
}
