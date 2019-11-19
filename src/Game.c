/*
 * Game.c
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <internal/Game.h>

#ifndef TIGAME_NO_DEBUG
#define debug(expn) expn
#else
#define debug(expn)
#endif

#define GAME_VERSION 0x0000

#include <Random.h>
#include <tigame/Map.h>
#include <tigame/LinkedList.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct Extension{
	tigame_version version;
	const char* name;
	void(*cleanup_fn)(Game*,Extension*);
	void(*entryPoint)(Game*,Extension*);
	void* data;
};

struct ExtensionList{
	Extension* extension;
	ExtensionList* next;
};
struct GameData{
	tigame_version version;
	ExtensionList* extensions;
	ExtensionList* extensionsTail;
	Extension* tileDispatcher;
	const struct TileDispatcherCalls* tileCalls;
	Extension* itemDispatcher;
	const struct ItemDispatcherCalls* itemCalls;
	Extension* foodDispatcher;
	const struct FoodDispatcherCalls* foodCalls;
	debug(int floating_allocations;)
};

struct ItemStack{
	Item* item;
	uint8_t count;
	void* data;
};

struct Player{
	uint8_t health;
	uint8_t hydration;
	Position location;
	uint8_t bagSize;
	ItemStack* items[40];
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

void tigame_Game_setTileDispatcher(Game* game,Extension* ext,const struct TileDispatcherCalls* calls){
	struct GameData* data = getGameData(game);
	data->tileDispatcher = ext;
	data->tileCalls = calls;
}
void tigame_Game_setItemDispatcher(Game* game,Extension* ext,const struct ItemDispatcherCalls* calls){
	struct GameData* data = getGameData(game);
	data->itemDispatcher = ext;
	data->itemCalls = calls;
}
void tigame_Game_setFoodDispatcher(Game* game,Extension* ext,const struct FoodDispatcherCalls* calls){
	struct GameData* data = getGameData(game);
	data->foodDispatcher = ext;
	data->foodCalls = calls;
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
	(*game)->printf(game,"Loading extension with entrypoint %p\n",(void*)entry);
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

static void* getExtensionData(Game* game,Extension* ext){
	return ext->data;
}
static void setExtensionData(Game* game,Extension* ext,void* data){
	ext->data = data;
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

static Tile* newTiles(Game* game,const char* name,TileProperties properties){
	struct GameData* data = getGameData(game);
	return data->tileCalls->newTile(game,name,properties,data->tileDispatcher);
}

static void addTileEnterCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,Direction)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileEnterCallback(game,tile,callback,data->tileDispatcher);
}

static void addTileLeaveCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,Direction)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileLeaveCallback(game,tile,callback,data->tileDispatcher);
}

static void addTileGenerateCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Map*,Position)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileGenerateCallback(game,tile,callback,data->tileDispatcher);
}

static void addTilePlaceItemCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Map*,Position,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTilePlaceItemCallback(game,tile,callback,data->tileDispatcher);
}
static void addTileTickCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileTickCallback(game,tile,callback,data->tileDispatcher);
}

static void addTileNameLineCallback(Game* game,Tile* tile,const char*(*callback)(Game*,Random*,Player*,Map*,Position)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileNameLineCallback(game,tile,callback,data->tileDispatcher);
}
static void addTileDescriptionLineCallback(Game* game,Tile* tile,const char*(*callback)(Game*,Random*,Player*,Map*,Position)){
	struct GameData* data = getGameData(game);
	data->tileCalls->addTileDescriptionLineCallback(game,tile,callback,data->tileDispatcher);
}

static const TileProperties* getProperties(Game* game,const Tile* tile){
	struct GameData* data = getGameData(game);
	return data->tileCalls->getProperties(game,tile,data->tileDispatcher);
}

static const Tile* getTile(Game* game,const char* name){
	struct GameData* data = getGameData(game);
	return data->tileCalls->getTile(game,name,data->tileDispatcher);
}

static Item* newItem(Game* game,const char* name,ItemProperties properties){
	struct GameData* data = getGameData(game);
	return data->itemCalls->newItem(game,name,properties,data->itemDispatcher);
}

static void addItemUseCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->itemCalls->addItemUseCallback(game,item,callback,data->itemDispatcher);
}

static void addItemCollectCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->itemCalls->addItemCollectCallback(game,item,callback,data->itemDispatcher);
}

static void addItemNameCallback(Game* game,Item* item,const char*(*callback)(Game*,Random*,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->itemCalls->addItemNameCallback(game,item,callback,data->itemDispatcher);
}

static void addItemTickCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->itemCalls->addItemTickCallback(game,item,callback,data->itemDispatcher);
}

static void addItemGenerateCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*)){
	struct GameData* data = getGameData(game);
	data->itemCalls->addItemGenerateCallback(game,item,callback,data->itemDispatcher);
}

static const ItemProperties* getItemProperties(Game* game,const Item* item){
	struct GameData* data = getGameData(game);
	return data->itemCalls->getItemProperties(game,item,data->itemDispatcher);
}

static const Item* getItem(Game* game,const char* name){
	struct GameData* data = getGameData(game);
	return data->itemCalls->getItem(game,name,data->itemDispatcher);
}

static Food* newFood(Game* game,const char* name,FoodProperties properties){
	struct GameData* data = getGameData(game);
	return data->foodCalls->newFood(game,name,properties,data->foodDispatcher);
}

static const FoodProperties* getFoodProperties(Game* game,const Food* food){
	struct GameData* data = getGameData(game);
	return data->foodCalls->getFoodProperties(game,food,data->foodDispatcher);
}

static const Food* getFood(Game* game,const char* name){
	struct GameData* data = getGameData(game);
	return data->foodCalls->getFood(game,name,data->foodDispatcher);
}

/*
Food* (*newFood)(Game*,const char*,FoodProperties);
const FoodProperties* (*getFoodProperties)(Game*,const Food*);
const Food* (*getFood)(Game*,const char*);
*/

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
	setExtensionData,
	getExtensionData,
	NULL,
	NULL,
	NULL,
	newTiles,
	addTileEnterCallback,
	addTileLeaveCallback,
	addTileGenerateCallback,
	addTilePlaceItemCallback,
	addTileTickCallback,
	addTileNameLineCallback,
	addTileDescriptionLineCallback,
	getProperties,
	getTile,
	newItem,
	addItemUseCallback,
	addItemCollectCallback,
	addItemNameCallback,
	addItemTickCallback,
	addItemGenerateCallback,
	getItemProperties,
	getItem
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
	return structure;
}

void tigame_Game_cleanup(Game* game){
	struct GameData* data = getGameData(game);
	freeExtensions(data->extensions,game);
	debug(if(data->floating_allocations){
		(*game)->printf(game,"Warning: after cleanup of tigame, there are %d outstanding allocations that have not been freed.\n",data->floating_allocations);
		(*game)->printf(game,"These allocations may have leaked, or been freed through other means.\n");
	})
	(*game)->printf(game,"Goodbye\n");
	free(game);
}







