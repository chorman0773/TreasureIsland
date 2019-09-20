/*
 * Game.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_INTERNAL_GAME_H_
#define INCLUDE_INTERNAL_GAME_H_

#ifdef __cplusplus
extern"C"{
#endif
#include <tigame/Game.h>

Game* tigame_Game_allocateCOMStructure();
void tigame_Game_cleanup(Game*);
void tigame_Game_tickEpilouge(Game*);

#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_INTERNAL_GAME_H_ */
