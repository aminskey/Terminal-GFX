#ifndef TERMINALGFX_H
#define TERMINALGFX_H

#include <stdbool.h>

// defining arbitrary values for key-inputs

#define TGFX_KEY_UP     1001
#define TGFX_KEY_DOWN   1002
#define TGFX_KEY_LEFT   1003
#define TGFX_KEY_RIGHT  1004

// getting window size
#define tgfx_wsize wsize()

// Initialization & Shutdown
void tgfx_init();
void tgfx_terminate();

// Terminal Mode Control
void tgfx_cbreak();
void tgfx_nocbreak();

// Drawing & Screen Control
void tgfx_cls();
void tgfx_move_cursor(int y, int x);
void tgfx_toggleCursor(bool on);
void tgfx_clfpos();
void tgfx_save_pos();
void tgfx_mv_savedpos();

// Timing
void tgfx_tick(int rate);

// Input
void tgfx_startInput();
void tgfx_closeInput();
int  tgfx_readInput(int hold);

#endif // TERMINALGFX_H


