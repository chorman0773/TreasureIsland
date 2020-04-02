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

#include <Stream.h>

typedef struct Random Random;

typedef struct Tile Tile;
typedef struct Item Item;
typedef struct Food Food;
typedef struct Player Player;
typedef struct Map Map;
typedef struct ItemStack ItemStack;
typedef struct Extension Extension;


typedef struct Position{
	uint8_t x,y;
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
	RESULT_ALLOW = 0, RESULT_DENY = 1,
	RESULT_ERROR = 2
};

typedef struct TileProperties{
	TileType type;
	const char* humanName;
	char mapChar;
	void*(*allocDataFn)(Game*);
	void(*freeDataFn)(Game*,void*);
	int16_t generateWeight;
	int32_t tileColor;
} TileProperties;

typedef struct ItemProperties{
	const char* humanName;
	void*(*allocDataFn)(Game*);
	void(*freeDataFn)(Game*,void*);
	int16_t generateWeight;
	int8_t maxStackSize;
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

typedef struct RandomCalls{
	tigame_bool(*nextBoolean)(Random*);
	int32_t(*nextInt)(Random*);
	int32_t(*nextIntb)(Random*,int32_t);
	float(*nextFloat)(Random*);
	double(*nextDouble)(Random*);
	int64_t(*nextLong)(Random*);
	double(*nextGaussian)(Random*);
	void(*nextBytes)(Random*,void*,size_t);
} RandomCalls;


struct GameCalls{
	void* reserved1; //Reserved for COM Compatibility
	void* reserved2; //Set to NULL
	void* reserved3;
	void* reserved4;
	/**
	* Allocates data in an unspecified manner.
	*/
	void* (__attribute__((alloc_size(2)))* alloc)(Game* game,size_t size);
	/**
	 * frees data obtained by a call to alloc with the same game argument.
	 * If ptr is not NULL, it shall have been allocated by a call to alloc with the same argument, or the behavior is undefined.
	 */
	void  (*free)(Game* game,void* ptr);

	/**
	 * Prints a given format string in an unspecified manner.
	 * The String is formatted as though by a call to printf.
	 * @param fmt Format String for the output
	 * @param ... Format Arguements
	 */
	void (__attribute__((format(printf,2,3)))*printf)(Game* game,const char* fmt,...);
	/**
	 * Obtains a pointer to a structure that can be used to obtain random numbers, given a Pointer to a Random* otherwise passed.
	 */
	const RandomCalls* (*getRandomCalls)(Game*);
	/**
	 * Sets the name of the extension being initialized.
	 * It is recommended that 
	 */
	void (*setExtensionName)(Game*,Extension*,const char*);
	void (*setExtensionVersion)(Game*,Extension*,tigame_version);
	void (*setExtensionCleanupFn)(Game*,Extension*,void(*)(Game*,Extension*));
	ExtensionList* (*extensionsEnd)(Game*);
	ExtensionList* (*getExtensions)(Game*);
	ExtensionList* (*nextExtension)(Game*,ExtensionList*);
	const Extension* (*getExtensionAt)(Game*,ExtensionList*);
	const Extension* (*getExtension)(Game*,const char*);
	tigame_version (*getVersion)(Game*);
	Extension* (*loadExtension)(Game*,Extension_entryPoint*);
	void (*setExtensionDataStruct)(Game*,Extension*,void*);
	void* (*getExtensionDataStruct)(Game*,Extension*);
	void (*setExtensionDebugFunction)(Game*,Extension*,Extension_entryPoint*);
	void (*minimumRequired)(Game*,uint16_t);
	void (__attribute__((format(printf,2,3),noreturn))*unrecoverable)(Game*,const char*,...);
	Tile* (*newTile)(Game*,const char*,TileProperties);
	void(*addTileEnterCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction));
	void(*addTileLeaveCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction));
	void(*addTileGenerateCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Map*,Position));
	void(*addTilePlaceItemCallback)(Game*,Tile*,ActionResult(*)(Game*,Map*,Position,ItemStack*));
	void(*addTileTickCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position));
	void(*addTileNameLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position));
	void(*addTileDescriptionLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position));
	const TileProperties* (*getProperties)(Game*,const Tile*);
	const Tile* (*getTile)(Game*,const char*);
	Item* (*newItem)(Game*,const char*,ItemProperties);
	void (*addItemUseCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemCollectCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemNameCallback)(Game*,Item*,const char*(*)(Game*,Random*,ItemStack*));
	void (*addItemTickCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	void (*addItemGenerateCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*));
	const ItemProperties* (*getItemProperties)(Game*,const Item*);
	const Item* (*getItem)(Game*,const char*);
	Food* (*newFood)(Game*,const char*,FoodProperties);
	const FoodProperties* (*getFoodProperties)(Game*,const Food*);
	const Food* (*getFood)(Game*,const char*);
	void* reserved11;
	void* reserved12;
	void* reserved13;
	void* reserved14;
	const Tile* (*getTileAt)(Game*,Map*,Position);
	void* (*getTileDataAt)(Game*,Map*,Position);
	ItemStack* (*getItemAt)(Game*,Map*,Position);
	void* reserved15;
	const Item* (*getStackItem)(Game*,ItemStack*);
	int(*getStackCount)(Game*,ItemStack*);
	void*(*getStackData)(Game*,ItemStack*);
	ItemStack* (*getItemStack)(Game*,Player*,uint8_t);
	uint8_t (*getBagSize)(Game*,Player*);
	ActionResult (*useItem)(Game*,Player*,ItemStack*);
	ItemStack* (*mergeItems)(Game*,ItemStack*,ItemStack*);
	ItemStack* (*createEmptyStack)(Game*);
	Position (*getPlayerPosition)(Game*,Player*);
	uint8_t (*getPlayerHealth)(Game*,Player*);
	uint8_t (*getPlayerHydration)(Game*,Player*);
	float (*getPlayerSaturation)(Game*,Player*);
	ActionResult (*movePlayer)(Game*,Player*,Direction);
	void(*setPlayerName)(Game*,Player*,const char*);
	const char*(*getPlayerName)(Game*,Player*);
	void (*damagePlayer)(Game*,Player*,uint8_t);
	void (*healPlayer)(Game*,Player*,uint8_t);
	void (*healSaturation)(Game*,Player*,float);
	void (*healHydration)(Game*,Player*,uint8_t);
	tigame_bool (*initGame)(Game*,Random*);
	uint8_t (*getPlayLength)(Game*);
	void (*tick)(Game*);
    void* reserved20;
    void* reserved21;
    void* reserved22;
    void* reserved23;
};


#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_GAME_H_ */
