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



#ifdef __cplusplus
};
#endif

#endif /* INCLUDE_INTERNAL_GAME_H_ */
