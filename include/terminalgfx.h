#ifndef TERMINALGFX_H
#define TERMINALGFX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// defining arbitrary values for key-inputs

#define TGFX_KEY_UP     1001
#define TGFX_KEY_DOWN   1002
#define TGFX_KEY_LEFT   1003
#define TGFX_KEY_RIGHT  1004

// getting window size

extern struct winsize tgfx_w;


typedef struct {
  int x;
  int y;
  int w;
  int h;
  char **img;
} SPRITE;

extern SPRITE *screenBuffer;

// Initialization & Shutdown
void tgfx_settings_init();
void tgfx_init();
void tgfx_terminate();

// Terminal Mode Control
void tgfx_cbreak();
void tgfx_nocbreak();

// Drawing & Screen Control
#define tgfx_cls()               printf("\x1b[2J")
#define tgfx_clfpos()            printf("\x1b[J")
#define tgfx_move_cursor(x,y)    printf("\x1b[%d;%dH", (y), (x))
#define tgfx_toggleCursor(on)    ((on) ? printf("\x1b[?25h") : printf("\x1b[?25l"))
#define tgfx_save_pos()          printf("\x1b[s")
#define tgfx_mv_savedpos()       printf("\x1b[u")

// Timing
void tgfx_tick(int);

// Input
void tgfx_startInput();
void tgfx_closeInput();
void tgfx_input_init();
void tgfx_input_terminate();
int  tgfx_readInput(int hold); // hold key down if hold=1 

// Frame buffer
void tgfx_fb_init(int, int);
void tgfx_fb_quit();
void tgfx_fb_put(int, int, char);
void tgfx_fb_print(int, int, const char *);
void tgfx_fb_render();
void create_box(SPRITE*);

#define tgfx_fb_box() create_box(screenBuffer)


// Sprite Code
SPRITE *createSprite(int, int, int, int);
void sprite_fill(SPRITE*, char);
void sprite_blit(SPRITE*, SPRITE*);

char *utf8string(uint32_t);

#endif // TERMINALGFX_H


