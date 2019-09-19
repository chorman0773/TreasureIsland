/*
 * Game.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_GAME_H_
#define INCLUDE_GAME_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/ComTypes.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct Random Random;

typedef struct Tile Tile;
typedef struct Item Item;
typedef struct Food Food;
typedef struct Player Player;
typedef struct Map Map;
typedef struct ItemStack ItemStack;
typedef struct Extension Extension;



typedef struct Position{
	unsigned x,y;
}Position;



typedef const struct GameCalls* Game;

typedef void(Extension_entryPoint)(Game*,Extension*);

typedef int32_t TileType;

enum{
	TILE_NORMAL = 0x01, TILE_HAZARD =0x02,
	TILE_SPECIAL = 0x04, TILE_NO_ITEMS = 0x08,
	TILE_DIRECTIONAL =0x10, TILE_BARRIER = 0x20
};

typedef int32_t Direction;

enum{
	DIR_NORTH = 0, DIR_SOUTH = 1,
	DIR_EAST = 2, DIR_WEST = 3
};

typedef int32_t ActionResult;

enum{
	RESULT_ALLOw, RESULT_DENY
};

typedef struct TileProperties{
	TileType type;
	const char* humanName;
	char mapChar;
	void*(*allocDataFn)(Game*);
	void(*freeDataFn)(Game*,void*);
	int16_t generateWeight;
} TileProperties;

typedef struct ItemProperties{
	const char* humanName;
	void*(*allocDataFn)(Game*);
	void(*freeDataFn)(Game*,void*);
	int16_t generateWeight;
} ItemProperties;

typedef struct FoodProperties{
	const char* humanName;
	int spoilRate;
	double spoilChance;
	int healthHeal;
	float saturationHeal;
	int16_t generateWeight;
} FoodProperties;

typedef struct ExtensionList ExtensionList;

struct GameCalls{
	void* reserved1; //Reserved for COM Compatibility
	void* reserved2; //Set to NULL
	void* reserved3;
	void* reserved4;
	void* (*alloc)(Game*,size_t);
	void  (*free)(Game*,void*);
	void (*setExtensionName)(Game*,Extension*,const char*);
	void (*setExtensionVersion)(Game*,Extension*,tigame_version);
	void (*setExtensionCleanupFn)(Game*,Extension*,void(*)(Game*,Extension*));
	const Extension* (*getExtension)(const char*);
	ExtensionList* (*extensionsEnd)(Game*);
	ExtensionList* (*getExtensions)(Game*);
	ExtensionList* (*nextExtension)(Game*,ExtensionList*);
	const Extension* (*getExtensionAt)(Game*,ExtensionList*);
	void* reserved5; //Reserved for future versions.
	void* reserved6; //This may be non-NULL.
	void* reserved7;
	void* reserved8;
	void* reserved9;
	void* reserved10;
	tigame_version (*getVersion)(Game*);
	Tile* (*newTile)(Game*,const char*,TileProperties);
	void(*addTileEnterCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction));
	void(*addTileLeaveCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction));
	void(*addTileGenerateCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Map*,Position));
	void(*addTilePlaceItemCallback)(Game*,Tile*,ActionResult(*)(Game*,Map*,Position,ItemStack*));
	void(*addTileTickCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position));
	const TileProperties* (*getProperties)(Game*,const Tile*);
	const Tile* (*getTile)(Game*,const char*);
	Item* (*newItem)(Game*,const char*,ItemProperties);
	void (*addItemUseCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemCollectCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemNameCallback)(Game*,Item*,const char*(*)(Game*,Random*,ItemStack*));
	void (*addItemTickCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemGenerateCallback)(Game*,Item,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	const ItemProperties* (*getItemProperties)(Game*,const Item*);
	const Item* (*getItem)(Game*,const char*);
	Food* (*newFood)(Game*,const char*,FoodProperties);
	const FoodProperties* (*getFoodProperties)(Game*,const Food*);
	const Food* (*getFood)(const char*);
	void* reserved11;
	void* reserved12;
	void* reserved13;
	void* reserved14;
	const Tile* (*getTileAt)(Game*,Map*,Position);
	void* (*getTileDataAt)(Game*,Map*,Position);

};


#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_GAME_H_ */
