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
#define NDEBUG
#endif

#define GAME_VERSION 0x0000

#include <Random.h>
#include <tigame/Map.h>
#include <List.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

struct Extension{
	tigame_version version;
	const char* name;
	void(*cleanup_fn)(Game*,Extension*);
	void(*entryPoint)(Game*,Extension*);
	void(*debugFn)(Game*,Extension*);
	void* data;
};


struct GameData{
	tigame_version version;
	LinkedList* extensions;
	Extension* tileDispatcher;
	const struct TileDispatcherCalls* tileCalls;
	Extension* itemDispatcher;
	const struct ItemDispatcherCalls* itemCalls;
	Extension* foodDispatcher;
	const struct FoodDispatcherCalls* foodCalls;
	jmp_buf* landing;
	const char* volatile* msgStore;
	char errorMsg[256];
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

struct MapTile{
	const Tile* tile;
	void* data;
	ItemStack* stack;
};

struct Map{
	uint8_t length;
	uint8_t width;
	struct MapTile* tiles;
};

static struct GameData* getGameData(Game* game){
	return (struct GameData*)(game+1);
}

Map* tigame_Game_genMap(Game* game,Random* rand,uint8_t length,uint8_t width){
	struct GameData* data = getGameData(game);
	Map* map = (*game)->alloc(game,sizeof(Map));
	map->length = length;
	map->width = width;
	map->tiles = (*game)->alloc(game,sizeof(struct MapTile)*length*width);
	for(uint8_t x = 0;x<length;x++)
		for(uint8_t y = 0;y<width;y++){
			Position pos = {x,y};
			struct MapTile* tile = &map->tiles[y*length+x];
			tile->tile = data->tileCalls->pickTile(game,rand,data->tileDispatcher);
			const TileProperties* properties = data->tileCalls->getProperties(game,tile->tile,data->tileDispatcher);
			if(properties->allocDataFn)
				tile->data = properties->allocDataFn(game);
			else
				tile->data = NULL;
		}

	return map;
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
	Extension* ext = malloc(sizeof(Extension));
	ext->entryPoint = entry;
	ext->cleanup_fn = NULL;
	ext->debugFn = NULL;
	ext->name = NULL;
	ext->version = -1;
	ext->data = NULL;
    LinkedList_pushBack(getGameData(game)->extensions,ext);
	entry(game,ext);
	return ext;
}

static ExtensionList* extensionsEnd(Game* game){
	return NULL;
}

static ExtensionList* getExtensions(Game* game){
	return (ExtensionList*)LinkedList_begin(getGameData(game)->extensions);
}

static ExtensionList* nextExtension(Game* game,ExtensionList* list){
	return (ExtensionList*)LinkedList_next((Iterator*)list);
}
static const Extension* getExtensionAt(Game* game,ExtensionList* list){
	return (Extension*)LinkedList_dereference((Iterator*)list);
}

static void* getExtensionData(Game* game,Extension* ext){
	return ext->data;
}
static void setExtensionData(Game* game,Extension* ext,void* data){
	ext->data = data;
}

static const Extension* getExtension(Game* game,const char* name){
	for(Iterator* list=(Iterator*)getExtensions(game);list!=NULL;list=LinkedList_next(list)){
		const Extension* ext = LinkedList_dereference(list);
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

static void setDebugFn(Game* game,Extension* ext,Extension_entryPoint* debug_fn){
    ext->debugFn = debug_fn;
}

static void minimumRequired(Game* game,uint16_t ver){
    if(TIGAME_COM_ABI<ver)
        (*game)->unrecoverable(game,"Extension Targets the future. Expected Minimum Version %hx, running on %hx",ver,TIGAME_COM_ABI);
}

static __attribute__((format(printf,2,3),noreturn)) void unrecoverable(Game* game,const char* fmt,...){
    va_list list;
    va_start(list,fmt);
    struct GameData* data = getGameData(game);
    vsnprintf(data->errorMsg,256,fmt,list);
    va_end(list);
    if(data->landing) {
        *(data->msgStore) = data->errorMsg;
        longjmp(*data->landing, 1);
    }else{
        puts(data->errorMsg);
        abort();
    }
}



/*
Food* (*newFood)(Game*,const char*,FoodProperties);
const FoodProperties* (*getFoodProperties)(Game*,const Food*);
const Food* (*getFood)(Game*,const char*);
*/

static const struct GameCalls CALLS = {
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
    setDebugFn,
	minimumRequired,
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

	data->extensions = LinkedList_new(free);
	data->landing = NULL;
	Extension* comAPI = tigame_alloc(structure,sizeof(Extension));
	comAPI->version = TIGAME_COM_ABI;
	comAPI->name = "comapi";
	comAPI->entryPoint = NULL;
	comAPI->cleanup_fn = NULL;
	LinkedList_pushBack(data->extensions,comAPI);
	return structure;
}

void tigame_Game_cleanup(Game* game){
	struct GameData* data = getGameData(game);
	LinkedList_free(data->extensions);
	debug(if(data->floating_allocations){
		(*game)->printf(game,"Warning: after cleanup of tigame, there are %d outstanding allocations that have not been freed.\n",data->floating_allocations);
		(*game)->printf(game,"These allocations may have leaked, or been freed through other means.\n");
	})
	(*game)->printf(game,"Goodbye\n");
	free(game);
}

void tigame_Game_printExtensionInfo(Game* game){
	struct GameData* data = getGameData(game);
	
	for(Iterator* list = LinkedList_begin(data->extensions);list;list = LinkedList_next(list)){
		Extension* ext = LinkedList_dereference(list);
		(*game)->printf(game,"Extension Loaded: %s at version %hd. (Entry point %p, Cleanup %p, Data %p)\n",ext->name,ext->version,(void*)ext->entryPoint,(void*)ext->cleanup_fn,ext->data);
	    if(ext->debugFn)
	        ext->debugFn(game,ext);
	}
}


void tigame_Game_setErrorLandingPad(Game* game,jmp_buf* buf,const char*volatile*  msgStore){
    struct GameData* data = getGameData(game);
    data->msgStore = msgStore;
    data->landing = buf;
}


