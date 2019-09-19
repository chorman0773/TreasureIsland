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

static void* allocFoodItemStructure(Game* game){
	return (*game)->alloc(game,sizeof(FoodData));
}

static void freeFoodItemStructure(Game* game,void* data){
	return (*game)->free(game,data);
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

}

void Food_ExtensionMain(Game* game, Extension* ext){
	(*game)->setExtensionName(game,ext,"tigame_food_items");
	(*game)->setExtensionVersion(game,ext,0);
	(*game)->setExtensionCleanupFn(game,cleanup);
	Item* foodItem = (*game)->newItem(game,"food_item",FoodItemProperties);

}

