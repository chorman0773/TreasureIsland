/*
 * Game.c
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>

#ifndef TIGAME_NO_DEBUG
#define debug(expn) expn
#else
#define debug(expn)
#endif

#define GAME_VERSION 0x0000

#include <stdlib.h>
#include <stdio.h>

struct Extension{
	tigame_version version;
	const char* name;
	void(*cleanup_fn)(Game*,Extension*);
	void(*entryPoint)(Game*,Extension*);
};

struct ExtensionList{
	Extension* extension;
	ExtensionList* next;
};
struct GameData{
	tigame_version version;
	ExtensionList* extensions;


	debug(int floating_allocations);
};

static void freeExtensions(ExtensionList* list,Game* game){
	if(list!=NULL){
		list->extension->cleanup_fn(game,list->extension);
		 (*game)->free(game,list->extension);
		 freeExtensions(list->next,game);
		 (*game)->free(game,list);
	}
}

static struct GameData* getGameData(Game* game){
	return (struct GameData*)(game+1);
}

static void* tigame_alloc(Game* game,size_t sz){
	debug(getGameData(game)->floating_allocations++);
	return malloc(sz);
}

static void tigame_free(Game* game,void* v){
	debug(getGameData(game)->floating_allocations--);
	free(v);
}

struct GameCalls CALLS = {
		NULL,
		NULL,
		NULL,
		NULL,
		tigame_alloc,
		tigame_free
};

Game* tigame_Game_allocateCOMStructure(){
	Game* structure = malloc(sizeof(Game)+sizeof(struct GameData));

	return structure;
}

void tigame_Game_cleanup(Game* game){
	struct GameData* data = getGameData(game);
	freeExtensions(data->extensions,game);
	free(game);
}





