/*
 * ColoredWrite.h
 *
 *  Created on: Sep 21, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_INTERNAL_COLOREDWRITE_H_
#define INCLUDE_INTERNAL_COLOREDWRITE_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/Game.h>
#include <stdint.h>

void coloredWrite(Game* game,const char* c,int32_t color);


#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_INTERNAL_COLOREDWRITE_H_ */
