/*
 * ComTypes.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_TIGAME_COMTYPES_H_
#define INCLUDE_TIGAME_COMTYPES_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/ComAbi.h>

#include <stdint.h>

typedef uint8_t tigame_bool;

enum{
	TIGAME_TRUE =1,
	TIGAME_FALSE = 0
};

typedef uint32_t tigame_result;

enum{
	TIGAME_OK = 0
};


typedef uint16_t tigame_version;

#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_TIGAME_COMTYPES_H_ */
