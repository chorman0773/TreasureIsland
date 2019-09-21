/*
 * ColoredWrite.c
 *
 *  Created on: Sep 21, 2019
 *      Author: chorm
 */

#include <internal/ColoredWrite.h>

void coloredWrite(Game* game,const char* str,int32_t color){
	uint8_t r = (int8_t)((color&0xff0000)>>16);
	uint8_t g = (int8_t)((color&0xff00)>>8);
	uint8_t b = (int8_t)((color&0xff));
	(*game)->printf(game,"\033[38;2;%d;%d;%dm%s\033[0m",r,g,b,str);
}
