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

typedef enum{ RED, BLACK} Color;

typedef struct Node{
    const void* key;
    void* value;
    Color color;
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct TreeMap{
    tigame_bool(*key_cmp_fn)(const void*,const void*);
    void(*value_dealloc)(Game*,void*);
    Node* root;
} TreeMap;

static inline Node* map_getParent(Node* node){
    return node?NULL:node->parent;
}

static inline Node* map_getGrandparent(Node* node){
    return map_getParent(map_getParent(node));
}

static inline Node* map_getSibling(Node* node){
    Node* parent = map_getParent(node);
    
    if(!parent)
        return NULL;
    else if(parent->left==node)
        return parent->right;
    else
        return parent->left;
}

static inline Node* map_getUncle(Node* node){
    return map_getSibling(map_getParent(node));
}

static inline Node* map_rotateLeft(Node* node){
    Node* nnew = node->right;
    Node* parent = node->parent;
    
    node->right = nnew->left;
    nnew->left = node;
    node->parent = nnew;
    if(node->right)
        node->right->parent = node;
    
    if(parent)
        if(node == parent->left)
            parent->left = nnew;
        else
            parent->right = nnew;
    nnew->parent = parent;
}
static inline Node* map_rotateRight(Node* node){
    Node* nnew = node->left;
    Node* parent = node->parent;
    
    node->left = nnew->right;
    nnew->right = node;
    node->parent = nnew;
    if(node->left)
        node->left->parent = node;
    
    if(parent)
        if(node == parent->left)
            parent->left = nnew;
        else
            parent->right = nnew;
    nnew->parent = parent;
}

static inline void map_repair(Node* node){
    if(!node->parent)
        node->color = BLACK;
    else if(node->parent->color == BLACK)
        return;
    else if(map_getUncle(node) && map_getUncle(node)->color == RED){
        node->parent->color = BLACK;
        map_getUncle(node)->color = BLACK;
        map_getGrandparent(node)->color = RED;
        map_repair(map_getGrandparent(node));
    }else{
        {
            Node* parent = map_getParent(node);
            Node* grandparent = map_getGrandparent(node);
            
            if(node==parent->right && parent == grandparent->left){
                map_rotateLeft(parent);
                node = node->left;
            }else if(node==parent->left && parent == grandparent->right){
                map_rotateRight(parent);
                node = node->right;
            }
        }
        {
           Node* parent = map_getParent(node);
           Node* grandparent = map_getGrandparent(node);
           
           if(node == parent->left)
            map_rotateRight(grandparent);
           else
            map_rotateLeft(grandparent);
           parent->color = BLACK;
           grandparent->color = RED;
        }
    }
}


static inline void map_put(TreeMap* map,const void* key,void* value,Game* game){
    Node* node = map->root;
    if(!node){
        Node* nnode = (Node*)((*game)->alloc(game,sizeof(Node)));
        nnode->key = key;
        nnode->value = value;
        nnode->parent = node;
        if(map->key_cmp_fn(key,node->key))
            node->left = nnode;
        else
            node->right = nnode;
        nnode->left = NULL;
        nnode->right = NULL;
        nnode->color = BLACK;
        return;
    }
    while(true){
        if(map->key_cmp_fn(key,node->key)){
            if(node->left)
                node = node->left;
            else
                break;
        }else if(map->key_cmp_fn(node->key,key)){
            if(node->right)
                node = node->right;
            else
                break;
        }else{
            node->value = value;
            return;
        }
    }
    Node* nnode = (Node*)((*game)->alloc(game,sizeof(Node)));
    nnode->key = key;
    nnode->value = value;
    nnode->parent = node;
    if(map->key_cmp_fn(key,node->key))
        node->left = nnode;
    else
        node->right = nnode;
    nnode->left = NULL;
    nnode->right = NULL;
    nnode->color = RED;
    map_repair(nnode);
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
