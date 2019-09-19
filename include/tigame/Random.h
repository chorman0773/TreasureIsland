/*
 * Random.h
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_RANDOM_H_
#define INCLUDE_RANDOM_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
typedef struct Random Random;


Random* Random_new();
void Random_free(Random* rand);
void Random_seed(Random* rand);
void Random_setSeed(Random* rand,uint64_t seed);
int Random_nextInt(Random* rand);
int Random_nextIntb(Random* rand,unsigned bound);
int64_t Random_nextLong(Random* rand);
float Random_nextFloat(Random* rand);
double Random_nextDouble(Random* rand);
double Random_nextGaussian(Random* rand);
bool Random_nextBoolean(Random* rand);
void Random_nextBytes(Random* rand,void* out,size_t size);


#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_RANDOM_H_ */
