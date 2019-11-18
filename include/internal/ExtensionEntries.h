/*
 * ExtensionEntries.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_INTERNAL_EXTENSIONENTRIES_H_
#define INCLUDE_INTERNAL_EXTENSIONENTRIES_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/Game.h>

void tigame_ExtLoad_main(Game* game,Extension* ext);
void tigame_FoodItem_main(Game* game,Extension* ext);

#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_INTERNAL_EXTENSIONENTRIES_H_ */
