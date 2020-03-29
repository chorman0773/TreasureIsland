#include <tigame/Game.h>

void tigame_ExtensionMain(Game* game,Extension* ext){
    (*game)->printf(game,"Loading Standard Items at version %d\n",0);
    (*game)->setExtensionName(game,ext,"items");
    (*game)->setExtensionVersion(game,ext,0);
}
