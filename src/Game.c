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
	ExtensionList* extensionsTail;
	debug(int floating_allocations);
};

static void freeExtensions(ExtensionList* list,Game* game){
	if(list!=NULL){
		freeExtensions(list->next,game);
		if(list->extension->cleanup_fn)
			list->extension->cleanup_fn(game,list->extension);
		(*game)->free(game,list->extension);
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
	*structure = &CALLS;
	struct GameData* data = (struct GameData*)(structure+1);
	debug(data->floating_allocations = 0);
	data->version = GAME_VERSION;
	ExtensionList* list = tigame_alloc(structure,sizeof(ExtensionList));
	data->extensions = list;
	data->extensionsTail = list;
	list->next = NULL;
	Extension* comAPI = tigame_alloc(structure,sizeof(Extension));
	comAPI->version = TIGAME_COM_ABI;
	comAPI->name = "comapi";
	comAPI->entryPoint = NULL;
	comAPI->cleanup_fn = NULL;
	list->extension = comAPI;
	return structure;
}

void tigame_Game_cleanup(Game* game){
	struct GameData* data = getGameData(game);
	freeExtensions(data->extensions,game);
	free(game);
}

Extension* tigame_Game_loadExtension(Game* game,Extension_entryPoint* entry){
	ExtensionList* list = getGameData(game)->extensionsTail;
	list->next = tigame_alloc(game,sizeof(ExtensionList));
	list = list->next;
	getGameData(game)->extensionsTail = list;
	list->next = NULL;
	Extension* ext = tigame_alloc(game,sizeof(ExtensionList));
	ext->entryPoint = entry;
	ext->cleanup_fn = NULL;
	ext->name = NULL;
	ext->version = -1;
	entry(game,ext);
	list->extension = ext;
	return ext;
}





