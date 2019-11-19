#include <internal/Game.h>
#include <tigame/Game.h>
#include <tigame/Map.h>
#include <string.h>

/*
struct ItemDispatcherCalls{
	Item* (*newItem)(Game*,const char*,ItemProperties,Extension*);
	void (*addItemUseCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemCollectCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemNameCallback)(Game*,Item*,const char*(*)(Game*,Random*,ItemStack*),Extension*);
	void (*addItemTickCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	void (*addItemGenerateCallback)(Game*,Item*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension*);
	const ItemProperties* (*getItemProperties)(Game*,const Item*,Extension*);
	const Item* (*getItem)(Game*,const char*,Extension*);
};
*/

struct Item{
    const char* name;
    ItemProperties properties;
    ActionResult(*useCallback)(Game*,Random*,Player*,Map*,Position,ItemStack*);
    ActionResult(*collectCallback)(Game*,Random*,Player*,Map*,Position,ItemStack*);
    const char*(*nameCallback)(Game*,Random*,ItemStack*);
    ActionResult(*tickCallback)(Game*,Random*,Player*,Map*,Position,ItemStack*);
    ActionResult(*generateCallback)(Game*,Random*,Player*,Map*,Position,ItemStack*)
};

static tigame_bool strless(const void* a,const void* b){
	return strcmp((const char*)a,(const char*)b)<0;
}

static Item* newItem(Game* game,const char* name,ItemProperties properties,Extension* ext){
    (*game)->printf(game,"Registering item with name: %s\n",name);
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    Item* item = (Item*) (*game)->alloc(game,sizeof(Item));
    item->name = name;
    item->properties = properties;
    map_put(map,name,item,game);
    return item;
}

static void addItemUseCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension* ext){
    item->useCallback = callback;
}

static void addItemCollectCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension* ext){
    item->collectCallback = callback;
}

static void addItemNameCallback(Game* game,Item* item,const char*(*callback)(Game*,Random*,ItemStack*),Extension* ext){
    item->nameCallback = callback;
}

static void addItemTickCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension* ext){
    item->tickCallback = callback;
}

static void addItemGenerateCallback(Game* game,Item* item,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,ItemStack*),Extension* ext){
    item->generateCallback = callback;
}

static const ItemProperties* getItemProperties(Game* game,const Item* item,Extension* ext){
    return &item->properties;
}
const Item* getItem(Game* game,const char* name,Extension* ext){
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    return (const Item*)map_get(map,name);
}

static const struct ItemDispatcherCalls DISPATCHER_CALLS = {
    newItem,
    addItemUseCallback,
    addItemCollectCallback,
    addItemNameCallback,
    addItemTickCallback,
    addItemGenerateCallback,
    getItemProperties,
    getItem
};

static void cleanup(Game* game,Extension* ext){
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    map_free(game,map);
    (*game)->printf(game,"Cleaned up Item Dispatcher Builtin\n");
}

void tigame_Items_main(Game* game,Extension* ext){
    (*game)->printf(game,"Initialized Items Dispatcher Builtin with version %hd\n",0);
    (*game)->setExtensionName(game,ext,"items");
    (*game)->setExtensionVersion(game,ext,0);
    (*game)->setExtensionCleanupFn(game,ext,cleanup);
    TreeMap* map = map_new(game,strless,(*game)->free);
    (*game)->setExtensionDataStruct(game,ext,map);
    tigame_Game_setItemDispatcher(game,ext,&DISPATCHER_CALLS);
}

