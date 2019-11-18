#include <tigame/Game.h>

static void cleanup(Game* game,Extension* ext){
    (*game)->printf(game,"Closing Test Extension\n");
} 

void tigame_ExtensionMain(Game* game,Extension* ext){
    (*game)->printf(game,"Loading Test Extension\n");
    (*game)->setExtensionName(game,ext,"test");
    (*game)->setExtensionVersion(game,ext,0);
    (*game)->setExtensionCleanupFn(game,ext,cleanup);
}
