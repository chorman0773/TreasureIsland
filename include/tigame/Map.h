/*
 * Map.h
 *
 *  Created on: Sep. 20, 2019
 *      Author: connor
 */

#ifndef INCLUDE_INTERNAL_MAP_H_
#define INCLUDE_INTERNAL_MAP_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/ComTypes.h>
#include <stddef.h>

typedef struct Node{
	const void* key;
	void* value;
	struct Node* left;
	struct Node* right;
} Node;

typedef struct TreeMap{
	tigame_bool(*key_cmp_fn)(const void*,const void*);
	void(*value_dealloc)(Game*,void*);
	Node* root;
} TreeMap;

static inline void map_put(TreeMap* map,const void* key,void* value,Game* game){
	Node* node = map->root;
	while(node){
		if(map->key_cmp_fn(key,node->key))
			node = node->left;
		else if(map->key_cmp_fn(node->key,key))
			node = node->right;
		else{
			node->value = value;
			return;
		}
	}
	node = (Node*)((*game)->alloc(game,sizeof(Node)));
	node->key = key;
	node->value = value;
	node->left = NULL;
	node->right = NULL;
}

static inline void* map_get(TreeMap* map,const void* key){
	Node* node = map->root;
	while(node){
		if(map->key_cmp_fn(key,node->key))
			node = node->left;
		else if(map->key_cmp_fn(node->key,key))
			node = node->right;
		else
			return node->value;
	}
	return NULL;
}

static inline TreeMap* map_new(Game* game,tigame_bool(*cmpFn)(const void*,const void*),void(*value_dtor)(Game*,void*)){
	TreeMap* map = (TreeMap*)(*game)->alloc(game,sizeof(TreeMap));
	map->root = NULL;
	map->key_cmp_fn = cmpFn;
	map->value_dealloc = value_dtor;
	return map;
}

static inline void node_free(Node* node,Game* game,TreeMap* map){
	if(node){
		node_free(node->left,game,map);
		node_free(node->right,game,map);
		map->value_dealloc(game,node->value);
		(*game)->free(game,node);
	}
}

static inline void map_free(Game* game,TreeMap* map){
	node_free(map->root,game,map);
	(*game)->free(game,map);
}

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_INTERNAL_MAP_H_ */
