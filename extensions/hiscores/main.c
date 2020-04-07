//
// Created by chorm on 2020-03-29.
//

#include <tigame/Game.h>
#include <ShadeNBT.h>
#include <ShadeConstants.h>

#include <Stream.h>
#include <BinaryIO.h>
#include <tigame/ComAbi.h>
#include <setjmp.h>
#include <string.h>


const char tigame_ExtensionName[] = "hiscores";
const uint16_t tigame_ExtensionVersion = 0;
const uint16_t tigame_MinimumVersion = TIGAME_COM_ABI;



void tigame_ExtensionCleanup(Game* game,Extension* ext){
    ShadeFile* file = (*game)->getExtensionDataStruct(game,ext);
    OutputStream* stream = OutputStream_openFilename("hiscores.ssv");
    BinaryIO* io = wrapOutputStream(stream,BINARY_BIG_ENDIAN);
    Shade_Write(file,io);
    Shade_Free(file);
    BinaryIO_free(io);
    OutputStream_destroy(stream);
}

static void io_error(void* userdata,const char* error){
    Game* game = (Game*)userdata;
    (*game)->unrecoverable(game,"Game IO Error: %s",error);
}

void tigame_ExtensionMain(Game* game,Extension* ext){
    (*game)->printf(game,"Hiscore Loading Controller Loaded with version %d",tigame_ExtensionVersion);
    (*game)->setExtensionName(game,ext,"hiscores");
    (*game)->setExtensionVersion(game,ext,0);
    InputStream* stream = InputStream_openFilename("hiscores.ssv");
    ShadeFile* file;
    if(stream) {
        BinaryIO* io =  wrapInputStream(stream,BINARY_BIG_ENDIAN);
        setErrorHandler(io,&io_error,game,NULL);
        file = Shade_Open(io);
        BinaryIO_free(io);
        InputStream_destroy(stream);
    }else{
        file = Shade_NewFile(v1_3,0);
    }
    (*game)->setExtensionDataStruct(game,ext,file);
}
