#include "terminalgfx.h"

void tgfx_init(){
  tgfx_settings_init();
  tgfx_input_init();
  tgfx_save_pos();
}
void tgfx_terminate(){
  tgfx_closeInput();
  tgfx_input_terminate();
  tgfx_fb_quit();
  tgfx_nocbreak();
  tgfx_move_cursor(0, 0);
  tgfx_clfpos();
  tgfx_mv_savedpos();
}
