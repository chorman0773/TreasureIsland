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
#include <List.h>

struct ExtLoaderData{
	LinkedList* list;
};



static void cleanup(Game* game,Extension* ext){
    struct ExtLoaderData* data = (struct ExtLoaderData*) ((*game)->getExtensionDataStruct(game,ext));
    LinkedList_free(data->list);
    (*game)->printf(game,"Closed ExtensionLoader builtin\n");
}
static void load_extension(const char* relpath,Game* game,struct ExtLoaderData* ext);
static void find_extensions(Game* game,struct ExtLoaderData* ext);

static void close(void* dso){
    dlclose(dso);
}

void tigame_ExtLoad_main(Game* game,Extension* ext){
    (*game)->printf(game,"Loading ExtensionLoader builtin\n");
    (*game)->setExtensionName(game,ext,"extld");
    (*game)->setExtensionVersion(game,ext,0);
    (*game)->setExtensionCleanupFn(game,ext,cleanup);
    struct ExtLoaderData* data = (struct ExtLoaderData*) (*game)->alloc(game,sizeof(struct ExtLoaderData));
    data->list = LinkedList_new(close);
    find_extensions(game,data);
    (*game)->setExtensionDataStruct(game,ext,data);
}


static void find_extensions(Game* game,struct ExtLoaderData* ext){
    DIR* dir = opendir("./extensions");
    char name[14+NAME_MAX] = "./extensions/";
    struct dirent* ent;
    while((ent = readdir(dir)))
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
        const uint16_t* minver = dlsym(handle,"tigame_MinimumVersion");
        if(minver)
            (*game)->minimumRequired(game,*minver);
        LinkedList_pushBack(ext->list,handle);
        Extension* ext = tigame_Game_loadExtension(game,entry);
        Extension_entryPoint* cleanup = (Extension_entryPoint*)dlsym(handle,"tigame_ExtensionCleanup");
        if(cleanup)
            (*game)->setExtensionCleanupFn(game,ext,cleanup);
        const char* name = (const char*)dlsym(handle,"tigame_ExtensionName");
        if(name)
            (*game)->setExtensionName(game,ext,name);
        const uint16_t* ver = dlsym(handle,"tigame_ExtensionVersion");
        if(ver)
            (*game)->setExtensionVersion(game,ext,*ver);
    }else
        dlclose(handle);
}

