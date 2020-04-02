/*
 * Main.c
 *
 *  Created on: Sep 18, 2019
 *      Author: chorm
 */

#include <tigame/Game.h>
#include <internal/Game.h>
#include <Random.h>
#include <internal/ColoredWrite.h>
#include <internal/ExtensionEntries.h>
#include <ConInteract.h>


int main(){
    const char* menu_items[] = {"Play Game","Load Game","View Scores","Debug","Exit",0};
    Random* rand = Random_new();
    jmp_buf landing;
    const char* volatile msg;
    Game* game = tigame_Game_allocateCOMStructure();
    if(setjmp(landing)){
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
        (*game)->printf(game,"Unrecoverable Game error: %s",msg);
#pragma clang diagnostic pop
    }
    tigame_Game_setErrorLandingPad(game,&landing,&msg);
    Random_seed(rand);
    (*game)->printf(game,"Treasure Island Game: Initialized with Version %hd\n",(*game)->getVersion(game));
    (*game)->loadExtension(game,tigame_Tiles_main);
    (*game)->loadExtension(game,tigame_Items_main);
    (*game)->loadExtension(game,tigame_FoodItem_main);
    (*game)->loadExtension(game,tigame_ExtLoad_main);
    waitInput();
    clearScreen();
    for(;;){
        switch(menu("Treasure Island Game",menu_items)){
            case 0:
                clearScreen();
                (*game)->printf(game,"Error: Playing the game is not implemented yet\n");
                waitch();
            break;
            case 1:
                clearScreen();
                (*game)->printf(game,"Error: Loading Game Files is not implemented yet\n");
                waitch();
            break;
            case 2:
                clearScreen();
                (*game)->printf(game,"There are no high scores, and no high score file is loaded\n");
                waitch();
            break;
            case 3:
                clearScreen();
                tigame_Game_printExtensionInfo(game);
                waitch();
            break;
            case 4:
            case MENU_CLOSE:
                goto exit;
            break;
        }
    }
    exit:
    clearScreen();
    tigame_Game_cleanup(game);
    Random_free(rand);
}
