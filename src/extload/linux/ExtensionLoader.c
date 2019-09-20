/*
 * ExtensionLoader.c
 *
 *  Created on: Sep. 20, 2019
 *      Author: connor
 */

#include <dlfcn.h>
#include <tigame/Game.h>
#include <internal/Game.h>

struct ExtensionDlList{
	void* handle;
	struct ExtensionDlList* next;
};

static struct ExtensionDlList* first = 0;
static struct ExtensionDlList* end = 0;

static void free_extdllist(Game* game,struct ExtensionDlList* list){
	if(list){
		free_extdllist(game,list->next);
		dlclose(list->handle);
		(*game)->free(game,list);
	}
}

static void cleanup(Game* game,Extension* ext){
	free_extdllist(game,first);
}

void tigame_ExtLoad_main(Game* game,Extension* ext){
	(*game)->setExtensionName(game,ext,"extld");
	(*game)->setExtensionVersion(game,ext,0);
	(*game)->setExtensionCleanupFn(game,ext,cleanup);

}


static void load_extension(const char* localpath,Game* game){
	void* handle = dlopen(localpath,RTLD_LAZY);
	if(!handle)
		return;
	Extension_entryPoint* entry = (Extension_entryPoint*) dlsym(handle,"tigame_ExtensionMain");
	if(entry){
		if(!end){
			end = (struct ExtensionDlList*)(*game)->alloc(game,sizeof(struct ExtensionDlList));
			first =end;
		}else{
			struct ExtensionDlList* next = (struct ExtensionDlList*)(*game)->alloc(game,sizeof(struct ExtensionDlList));
			end->next = next;
			end = next;
		}
		end->next = NULL;
		end->handle = handle;
		tigame_Game_loadExtension(game,entry);
	}

}

