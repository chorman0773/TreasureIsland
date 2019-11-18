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
static void load_extension(const char* relpath,Game* game);
static void find_extensions(Game* game);

void tigame_ExtLoad_main(Game* game,Extension* ext){
    (*game)->printf(game,"Loading ExtensionLoader builtin\n");
    (*game)->setExtensionName(game,ext,"extld");
    (*game)->setExtensionVersion(game,ext,0);
    (*game)->setExtensionCleanupFn(game,ext,cleanup);
    find_extensions(game);
}


static void find_extensions(Game* game){
    DIR* dir = opendir("./extensions");
    char name[14+NAME_MAX] = "./extensions/";
    struct dirent* ent;
    while(ent = readdir(dir))
        if(strstr(ent->d_name,".so")){
            strcpy(name+13,ent->d_name);
            load_extension(name,game);
        }
}

static void load_extension(const char* relpath,Game* game){
    (*game)->printf(game,"Loading extensions %s\n",relpath);
    void* handle = dlopen(relpath,RTLD_LAZY);
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

