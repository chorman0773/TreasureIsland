#include <internal/Game.h>
#include <tigame/Game.h>
#include <tigame/Map.h>
#include <string.h>

/**
struct TileDispatcherCalls{
	Tile*(*newTile)(Game*,const char* name,TileProperties props,Extension*);
	void(*addTileEnterCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction),Extension*);
	void(*addTileLeaveCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position,Direction),Extension*);
	void(*addTileGenerateCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Map*,Position),Extension*);
	void(*addTilePlaceItemCallback)(Game*,Tile*,ActionResult(*)(Game*,Map*,Position,ItemStack*),Extension*);
	void(*addTileTickCallback)(Game*,Tile*,ActionResult(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	void(*addTileNameLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	void(*addTileDescriptionLineCallback)(Game*,Tile*,const char*(*)(Game*,Random*,Player*,Map*,Position),Extension*);
	const TileProperties* (*getProperties)(Game*,const Tile*,Extension*);
	const Tile* (*getTile)(Game*,const char*,Extension*);
};
*/

struct Tile{
    const char* name;
    TileProperties properties;
    ActionResult(*enterCallback)(Game*,Random*,Player*,Map*,Position,Direction);
    ActionResult(*leaveCallback)(Game*,Random*,Player*,Map*,Position,Direction);
    ActionResult(*generateCallback)(Game*,Random*,Map*,Position);
    ActionResult(*placeItemCallback)(Game*,Map*,Position,ItemStack*);
    ActionResult(*tickCallback)(Game*,Random*,Player*,Map*,Position);
    const char*(*namelineCallback)(Game*,Random*,Player*,Map*,Position);
    const char*(*descriptionlineCallback)(Game*,Random*,Player*,Map*,Position);
};

static ActionResult pass_enter(Game* game,Random* rand,Player* player,Map* map,Position pos,Direction dir){
    return RESULT_ALLOW;
}

static ActionResult pass_generate(Game* game,Random* rand,Map* map,Position pos){
    return RESULT_ALLOW;
}

static ActionResult pass_placeItem(Game* game,Map* map,Position pos,ItemStack* stack){
    return RESULT_ALLOW;
}

static ActionResult pass_tick(Game* game,Random* rand,Player* player,Map* map,Position pos){
    return RESULT_ALLOW;
}

static const char* pass_nameline(Game* game,Random* rand,Player* player,Map* map,Position pos){
    return (*game)->getTileAt(game,map,pos)->properties.humanName;
}

static const char* pass_descriptionline(Game* game,Random* rand,Player* player,Map* map,Position pos){
    return "";
}

static tigame_bool strless(const void* a,const void* b){
	return strcmp((const char*)a,(const char*)b)<0;
}

Tile* newTile(Game* game,const char* name,TileProperties properties,Extension* ext){
    (*game)->printf(game,"Adding new tile with name %s\n",name);
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    Tile* tile = (Tile*) (*game)->alloc(game,sizeof(Tile));
    tile->name = name;
    tile->properties = properties;
    tile->enterCallback = pass_enter;
    tile->leaveCallback = pass_enter;
    tile->generateCallback = pass_generate;
    tile->placeItemCallback = pass_placeItem;
    tile->tickCallback = pass_tick;
    tile->namelineCallback = pass_nameline;
    tile->descriptionlineCallback = pass_descriptionline;
    map_put(map,name,tile,game);
    return tile;
}

static void addTileEnterCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,Direction),Extension* ext){
    tile->enterCallback = callback;
}
static void addTileLeaveCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position,Direction),Extension* ext){
    tile->leaveCallback = callback;
}
static void addTileGenerateCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Map*,Position),Extension* ext){
    tile->generateCallback = callback;
}
static void addTilePlaceItemCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Map*,Position,ItemStack*),Extension* ext){
    tile->placeItemCallback = callback;
}
static void addTileTickCallback(Game* game,Tile* tile,ActionResult(*callback)(Game*,Random*,Player*,Map*,Position),Extension* ext){
    tile->tickCallback = callback;
}
static void addTileNameLineCallback(Game* game,Tile* tile,const char*(*callback)(Game*,Random*,Player*,Map*,Position),Extension* ext){
    tile->namelineCallback = callback;
}
static void addTileDescriptionLineCallback(Game* game,Tile* tile,const char*(*callback)(Game*,Random*,Player*,Map*,Position),Extension* ext){
    tile->descriptionlineCallback = callback;
}
static const TileProperties* getProperties(Game* game,const Tile* tile,Extension* ext){
    return &tile->properties;
}
static const Tile* getTile(Game* game,const char* name,Extension* ext){
    TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
    return (const Tile*)map_get(map,name);
}

static const struct TileDispatcherCalls TILE_CALLS = {
    newTile,
    addTileEnterCallback,
    addTileLeaveCallback,
    addTileGenerateCallback,
    addTilePlaceItemCallback,
    addTileTickCallback,
    addTileNameLineCallback,
    addTileDescriptionLineCallback,
    getProperties
};

static void cleanup(Game* game,Extension* ext){
	TreeMap* map = (TreeMap*) (*game)->getExtensionDataStruct(game,ext);
	map_free(game,map);
	(*game)->printf(game,"Cleaned up Tiles builtin\n");
}

void tigame_Tiles_main(Game* game,Extension* ext){
	(*game)->printf(game,"Loading Tiles builtin\n");
	(*game)->setExtensionName(game,ext,"tiles_dispatcher");
	(*game)->setExtensionVersion(game,ext,0);
	(*game)->setExtensionCleanupFn(game,ext,cleanup);
	TreeMap* map = map_new(game,strless,(*game)->free);
	(*game)->setExtensionDataStruct(game,ext,map);
	tigame_Game_setTileDispatcher(game,ext,&TILE_CALLS);
}
