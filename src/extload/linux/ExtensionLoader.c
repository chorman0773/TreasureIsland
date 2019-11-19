/*
 * ExtensionLoader.c
 *
 *  Created on: Sep. 20, 2019
 *      Author: connor
 */

#include <dlfcn.h>
#include <tigame/Game.h>
#include <internal/Game.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

struct ExtensionDlList{
	void* handle;
	struct ExtensionDlList* next;
};

struct ExtLoaderData{
	struct ExtensionDlList* first;
	struct ExtensionDlList* end
};



static void free_extdllist(Game* game,struct ExtensionDlList* list){
    if(list){
        free_extdllist(game,list->next);
        dlclose(list->handle);
        (*game)->free(game,list);
    }
}

static void cleanup(Game* game,Extension* ext){
    struct ExtLoaderData* data = (struct ExtLoaderData*) ((*game)->getExtensionDataStruct(game,ext));
    free_extdllist(game,data->first);
}
static void load_extension(const char* relpath,Game* game,struct ExtLoaderData* ext);
static void find_extensions(Game* game,struct ExtLoaderData* ext);

void tigame_ExtLoad_main(Game* game,Extension* ext){
    (*game)->printf(game,"Loading ExtensionLoader builtin\n");
    (*game)->setExtensionName(game,ext,"extld");
    (*game)->setExtensionVersion(game,ext,0);
    (*game)->setExtensionCleanupFn(game,ext,cleanup);
    struct ExtLoaderData* data = (struct ExtLoaderData*) (*game)->alloc(game,sizeof(struct ExtLoaderData));
    find_extensions(game,data);
    (*game)->setExtensionDataStruct(game,ext,data);
}


static void find_extensions(Game* game,struct ExtLoaderData* ext){
    DIR* dir = opendir("./extensions");
    char name[14+NAME_MAX] = "./extensions/";
    struct dirent* ent;
    while(ent = readdir(dir))
        if(strstr(ent->d_name,".so")){
            strcpy(name+13,ent->d_name);
            load_extension(name,game,ext);
        }
}

static void load_extension(const char* relpath,Game* game,struct ExtLoaderData* ext){
    (*game)->printf(game,"Loading extensions %s\n",relpath);
    void* handle = dlopen(relpath,RTLD_LAZY);
    if(!handle)
        return;
    Extension_entryPoint* entry = (Extension_entryPoint*) dlsym(handle,"tigame_ExtensionMain");
    if(entry){
        if(!ext->end){
	        ext->end = (struct ExtensionDlList*)(*game)->alloc(game,sizeof(struct ExtensionDlList));
	        ext->first =ext->end;
        }else{
	        struct ExtensionDlList* next = (struct ExtensionDlList*)(*game)->alloc(game,sizeof(struct ExtensionDlList));
	        ext->end->next = next;
	        ext->end = next;
        }
        ext->end->next = NULL;
        ext->end->handle = handle;
        tigame_Game_loadExtension(game,entry);
    }

}

