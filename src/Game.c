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

#include <internal/Random.h>
#include <internal/Map.h>
#include <internal/LinkedList.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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
	TreeMap* tiles;
	TreeMap* items;
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

static __attribute__((format(printf,2,3))) void tigame_printf(Game* game,const char* fmt,...) {
	va_list list;
	va_start(list,fmt);
	vprintf(fmt,list);
	va_end(list);
}

static void setExtensionName(Game* game,Extension* ext,const char* name){
	ext->name = name;
}

static void setExtensionVersion(Game* game,Extension* ext,tigame_version ver){
	ext->version = ver;
}

static void setExtensionCleanupFunction(Game* game,Extension* ext,void(*cleanupfn)(Game* game,Extension* ext)){
	ext->cleanup_fn = cleanupfn;
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
	list->extension = ext;
	entry(game,ext);
	return ext;
}

static ExtensionList* extensionsEnd(Game* game){
	return NULL;
}

static ExtensionList* getExtensions(Game* game){
	return getGameData(game)->extensions;
}

static ExtensionList* nextExtension(Game* game,ExtensionList* list){
	return list->next;
}
static const Extension* getExtensionAt(Game* game,ExtensionList* list){
	return list->extension;
}

static const Extension* getExtension(Game* game,const char* name){
	for(ExtensionList* list=getExtensions(game);list!=NULL;list=nextExtension(game,list)){
		const Extension* ext = list->extension;
		if(strcmp(ext->name,name)==0)
			return ext;
	}
	return NULL;
}

static tigame_version getVersion(Game* game){
	return getGameData(game)->version;
}

static tigame_bool COM_Random_nextBoolean(Random* rand){
	return Random_nextBoolean(rand)?TIGAME_TRUE:TIGAME_FALSE;
}

static const RandomCalls* getRandomCalls(Game* g){
	static const RandomCalls RAND_CALLS = {
			COM_Random_nextBoolean,
			Random_nextInt,
			Random_nextIntb,
			Random_nextFloat,
			Random_nextDouble,
			Random_nextLong,
			Random_nextGaussian,
			Random_nextBytes
	};
	return &RAND_CALLS;
}

static struct GameCalls CALLS = {
		NULL,
		NULL,
		NULL,
		NULL,
		tigame_alloc,
		tigame_free,
		tigame_printf,
		getRandomCalls,
		setExtensionName,
		setExtensionVersion,
		setExtensionCleanupFunction,
		extensionsEnd,
		getExtensions,
		nextExtension,
		getExtensionAt,
		getExtension,
		getVersion,
		tigame_Game_loadExtension,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
};

static tigame_bool strless(const void* a,const void* b){
	return strcmp((const char*)a,(const char*)b)<0;
}


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
	data->items = map_new(structure,strless,(*structure)->free);
	data->tiles = map_new(structure,strless,(*structure)->free);
	return structure;
}

void tigame_Game_cleanup(Game* game){
	struct GameData* data = getGameData(game);
	freeExtensions(data->extensions,game);
	map_free(game,data->items);
	map_free(game,data->tiles);
	debug(if(data->floating_allocations){
		(*game)->printf(game,"Warning: after cleanup of tigame, there are %d outstanding allocations that have not been freed.\n",data->floating_allocations);
		(*game)->printf(game,"These allocations may have leaked, or been freed through other means.\n");
	})
	free(game);
}







