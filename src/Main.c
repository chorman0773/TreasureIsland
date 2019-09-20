/*
 * Main.c
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <internal/Game.h>

int main(){
	Game* game = tigame_Game_allocateCOMStructure();


	tigame_Game_cleanup(game);
}
