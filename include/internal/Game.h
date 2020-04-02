/*
 * Game.h
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_INTERNAL_GAME_H_
#define INCLUDE_INTERNAL_GAME_H_

#ifdef __cplusplus
extern"C"{
#endif
#include <tigame/Game.h>
#include <stdint.h>
#include <setjmp.h>


struct TileDispatcherCalls{
	Tile*(*newTile)(Game*,const char* name,TileProperties props,Extension*);
	void(*addTileEnterCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction),Extension*);
	void(*addTileLeaveCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction),Extension*);
	void(*addTileGenerateCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Map*,Position),Extension*);
	void(*addTilePlaceItemCallback)(Game*,Tile*,ActionResult(*)(Game*,Map*,Position,ItemStack*),Extension*);
	void(*addTileTickCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	void(*addTileNameLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	void(*addTileDescriptionLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	const TileProperties* (*getProperties)(Game*,const Tile*,Extension*);
	const Tile* (*getTile)(Game*,const char*,Extension*);
	const Tile* (*pickTile)(Game*,Random*,Extension*);
	ActionResult (*generateTile)(Game*,Random*,Map*,Position,Extension*);
	ActionResult (*placeItemOn)(Game*,Random*,Map*,Position,Extension*);
};

struct ItemDispatcherCalls{
	Item* (*newItem)(Game*,const char*,ItemProperties,Extension*);
	void (*addItemUseCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemCollectCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemNameCallback)(Game*,Item*,const char*(*)(Game*,Random*,ItemStack*),Extension*);
	void (*addItemTickCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemGenerateCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	const ItemProperties* (*getItemProperties)(Game*,const Item*,Extension*);
	const Item* (*getItem)(Game*,const char*,Extension*);
	const Item* (*pickItem)(Game*,Random*,Extension*);
	ActionResult (*generateItem)(Game*,Random*,ItemStack*,Extension*);
};

struct FoodDispatcherCalls{
	Food* (*newFood)(Game*,const char*,FoodProperties,Extension*);
	const FoodProperties* (*getFoodProperties)(Game*,const Food*,Extension*);
	const Food* (*getFood)(Game*,const char*,Extension*);
};

Game* tigame_Game_allocateCOMStructure();
void tigame_Game_cleanup(Game*);
void tigame_Game_tickEpilouge(Game*);

void tigame_Game_setTileDispatcher(Game* game,Extension* ext,const struct TileDispatcherCalls* calls);
void tigame_Game_setItemDispatcher(Game* game,Extension* ext,const struct ItemDispatcherCalls* calls);
void tigame_Game_setFoodDispatcher(Game* game,Extension* ext,const struct FoodDispatcherCalls* calls);

Extension* tigame_Game_loadExtension(Game* game,Extension_entryPoint* entrypoint);

void tigame_Game_printExtensionInfo(Game* game);

void tigame_Game_setErrorLandingPad(Game* game,jmp_buf* buf,const char*volatile*  msgStore);

Map* tigame_Game_genMap(Game* game,Random* rand,uint8_t len,uint8_t width);
void tigame_Game_mapFree(Game* game,Map* map);

#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_INTERNAL_GAME_H_ */
