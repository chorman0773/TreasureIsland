/*
 * LinkedList.h
 *
 *  Created on: Sep. 20, 2019
 *      Author: connor
 */

#ifndef INCLUDE_INTERNAL_LINKEDLIST_H_
#define INCLUDE_INTERNAL_LINKEDLIST_H_

#ifdef __cplusplus
extern"C"{
#endif

#include <tigame/Game.h> //for Game and game allocations, since


typedef struct LinkedList{
	void* data;
	struct LinkedList* next;
} LinkedList;

inline void freeList(LinkedList* list,Game* game){
	if(list){
		freeList(list->next,game);
		(*game)->free(game,list->data);
		(*game)->free(game,list);
	}
}

inline void listInsert(LinkedList* list,void* data,Game* game){
	LinkedList* next = (LinkedList*)((*game)->alloc(game,sizeof(LinkedList)));
	next->next = list->next;
	list->next = next;
	next->data = data;
}

inline LinkedList* listNext(LinkedList* list){
	return list->next;
}

void* listDereference(LinkedList* list){
	return list->data;
}

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_INTERNAL_LINKEDLIST_H_ */
