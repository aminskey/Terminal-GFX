#include "terminalgfx.h"


void tgfx_init(){
    tgfx_settings_init();
    tgfx_input_init();
}
void tgfx_terminate(){
    tgfx_closeInput();
    pthread_mutex_destroy(&inputLock);
    tgfx_nocbreak();
}