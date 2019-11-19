/*
 * Food.c
 *
 *  Created on: Sep 19, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <stdlib.h>

typedef struct FoodData{
	int spoilStage;
	Food* food;
} FoodData;

typedef struct FoodList{
	Food* food;
	struct FoodList* next;
} FoodList;

struct Food{
	FoodProperties properties;
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

static FoodList _food_list_begin = {NULL,NULL};

static void free_list(Game* game,FoodList* list){
	if(list!=NULL)
		free_list(game,list->next);
	(*game)->free(game,list);
}

static void cleanup(Game* game,Extension* ext){
	free_list(game,_food_list_begin.next);
}

static const char* food_name(Game* game,Random* rand,ItemStack* stack){
	return "";
}

void tigame_FoodItem_main(Game* game, Extension* ext){
	(*game)->printf(game,"Initialized Food Items Built-in with version %hd",0);
	(*game)->setExtensionName(game,ext,"tigame_food_items");
	(*game)->setExtensionVersion(game,ext,0);
	(*game)->setExtensionCleanupFn(game,ext,cleanup);
	Item* foodItem = (*game)->newItem(game,"food_item",FoodItemProperties);
	(*game)->addItemNameCallback(game,foodItem,food_name);
}

