/*
 * Random.c
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#include <tigame/Random.h>

#include <stdlib.h>
#include <stdatomic.h>
#include <math.h>
#include <time.h>

#ifdef TIGAME_MULTITHREAD_SUPPORT

#include <threads.h>
#define multithread_support(expn) expn
#else
#define multithread_support(expn)
#endif

struct Random{
	multithread_support(mtx_t lock;)
	uint64_t seed;
	bool haveNextNextGaussian;
	double nextNextGaussian;
	bool has_seeded;
};

static uint64_t genUniqueSeed(){
	static atomic_uint_fast64_t num = ATOMIC_VAR_INIT(319145242569417893uLL);
	static const uint64_t cnum = 11896147423605525413uLL;
	uint64_t val = atomic_load(&num);
	while(atomic_compare_exchange_strong(&num,&val,cnum*val+1));
	return 31*val+time(NULL);
}

static uint64_t initRandomizeSeed(uint64_t seed){
	return (seed ^ 0x5DEECE66DuLL) & ((1uLL << 48) - 1);
}

Random* Random_new(){
	Random* rand = malloc(sizeof(Random));
	rand->has_seeded = false;
	multithread_support(if(mtx_init(&rand->lock,mtx_plain|mtx_recursive)!=thrd_success){
		free(rand);
		return NULL;
	})
	rand->haveNextNextGaussian = false;
	return rand;
}

void Random_free(Random* rand){
	multithread_support(mtx_destroy(&rand->lock));
	free(rand);
}

void Random_setSeed(Random* rand,uint64_t seed){
	multithread_support(mtx_lock(&rand->lock);)
	rand->has_seeded = true;
	rand->seed = initRandomizeSeed(seed);
	rand->nextNextGaussian = false;
	multithread_support(mtx_unlock(&rand->lock);)
}

void Random_seed(Random* rand){
	Random_setSeed(rand,genUniqueSeed());
}

static uint32_t Random_next(Random* rand,int bits){
	uint32_t val;
	{
		multithread_support(mtx_lock(&rand->lock);)
		if(!rand->has_seeded)
			Random_seed(rand);
		rand->seed = (rand->seed * 0x5DEECE66DuLL + 0xBuLL) & ((1uLL << 48) - 1);
		val = (uint32_t)(rand->seed >> (48 - bits));
		multithread_support(mtx_unlock(&rand->lock);)
	}
	return val;
}

int Random_nextInt(Random* rand){
	return (int)(Random_next(rand,32));
}

int Random_nextIntb(Random* rand,unsigned bound){

   if ((bound & -bound) == bound)  // i.e., bound is a power of 2
	 return (int)((bound * (uint64_t)Random_next(rand,31)) >> 31);

   int bits, val;
   do {
	   bits = Random_next(rand,31);
	   val = bits % bound;
   } while (bits - val + (bound-1) < 0);
   return val;
}

int64_t Random_nextLong(Random* rand){
	return (int64_t)(((uint64_t)Random_next(rand,32)<<32)|Random_next(rand,32));
}

float Random_nextFloat(Random* rand){
	return Random_next(rand,26)/((float)(1<<27));
}

double Random_nextDouble(Random* rand){
	return (((uint64_t)Random_next(rand,26) << 27) + Random_next(rand,27))
		     / (double)(1L << 53);
}

bool Random_nextBoolean(Random* rand){
	return Random_next(rand,1)!=0;
}

double Random_nextGaussian(Random* rand){
	double val;
	multithread_support(mtx_lock(&rand->lock);)
	{
	   if (rand->haveNextNextGaussian) {
		 rand->haveNextNextGaussian = false;
		 val = rand->nextNextGaussian;
	   } else {
		 double v1, v2, s;
		 do {
		   v1 = 2 * Random_nextDouble(rand) - 1;   // between -1.0 and 1.0
		   v2 = 2 * Random_nextDouble(rand) - 1;   // between -1.0 and 1.0
		   s = v1 * v1 + v2 * v2;
		 } while (s >= 1 || s == 0);
		 double multiplier = sqrt(-2 * log(s)/s);
		 rand->nextNextGaussian = v2 * multiplier;
		 rand->haveNextNextGaussian = true;
		 val = v1 * multiplier;
	   }
	}
	multithread_support(mtx_unlock(&rand->lock);)
	return val;
}

