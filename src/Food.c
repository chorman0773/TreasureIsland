/*
 * Food.c
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#include <internal/Game.h>
#include <tigame/Game.h>
#include <tigame/Map.h>
#include <stdlib.h>
#include <string.h>

typedef struct FoodData{
	int spoilStage;
	Food* food;
} FoodData;



struct Food{
	FoodProperties properties;
	const char* name;
};

static void* allocFoodItemStructure(Game* game){
	return (*game)->alloc(game,sizeof(FoodData));
}

static void freeFoodItemStructure(Game* game,void* data){
	(*game)->free(game,data);
}

static const ItemProperties FoodItemProperties = {
		"This name should not appear",
		allocFoodItemStructure,
		freeFoodItemStructure,
		13
};

static tigame_bool strless(const void* a,const void* b){
	return strcmp((const char*)a,(const char*)b)<0;
}
 
/*
struct FoodDispatcherCalls{
	Food* (*newFood)(Game*,const char*,FoodProperties,Extension*);
	const FoodProperties* (*getFoodProperties)(Game*,const Food*,Extension*);
	const Food* (*getFood)(Game*,const char*,Extension*);
};
*/

static Food* newFood(Game* game,const char* name,FoodProperties properties,Extension* ext){
	(*game)->printf(game,"Registering food with name: %s\n",name);
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    Food* food = (Food*) (*game)->alloc(game,sizeof(Food));
    food->name = name;
    food->properties = properties;
    map_put(map,name,food,game);
    return food;
}

static const FoodProperties* getFoodProperties(Game* game,const Food* food,Extension* ext){
	return &food->properties;
}

static const Food* getFood(Game* game,const char* name,Extension* ext){
	TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
	return (const Food*) map_get(map,name);
}

const struct FoodDispatcherCalls FOOD_CALLS = {
	newFood,
	getFoodProperties,
	getFood
};

static void cleanup(Game* game,Extension* ext){
	TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
	map_free(game,map);
	(*game)->printf(game,"Cleaned up Food Items Built-in\n");
}

static const char* food_name(Game* game,Random* rand,ItemStack* stack){
	return "";
}

void tigame_FoodItem_main(Game* game, Extension* ext){
	(*game)->printf(game,"Initialized Food Items Built-in with version %hd\n",0);
	(*game)->setExtensionName(game,ext,"tigame_food_items");
	(*game)->setExtensionVersion(game,ext,0);
	(*game)->setExtensionCleanupFn(game,ext,cleanup);
	TreeMap* map = map_new(game,strless,(*game)->free);
	(*game)->setExtensionDataStruct(game,ext,map);
	Item* foodItem = (*game)->newItem(game,"food_item",FoodItemProperties);
	(*game)->addItemNameCallback(game,foodItem,food_name);
	tigame_Game_setFoodDispatcher(game,ext,&FOOD_CALLS);
}

