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

extern char WHITE[3];
extern char BLACK[3];

// defining colors
#define TGFX_COL_WHITE WHITE
#define TGFX_COL_BLACK BLACK

// getting window size
extern struct winsize tgfx_w;

typedef struct {
  char c;
  char glyph[5];  // 4 for UTF8 and 1 for \0
  unsigned char fRGB[3];
  unsigned char bRGB[3];
  char glyph_len;
} CELL;

typedef struct {
  int x;
  int y;
  int w;
  int h;
  CELL **img;
} SPRITE;

extern SPRITE *screenBuffer;

extern int tgfx_do_flush;

// Initialization & Shutdown
void tgfx_settings_init();
void tgfx_init();
void tgfx_terminate();

// Terminal Mode Control
void tgfx_cbreak();
void tgfx_nocbreak();
// Drawing & Screen Control
#define tgfx_cls()                printf("\x1b[2J")
#define tgfx_clfpos()             printf("\x1b[J")
#define tgfx_move_cursor(x,y)     printf("\x1b[%d;%dH", (y), (x))
#define tgfx_toggleCursor(on)     ((on) ? printf("\x1b[?25h") : printf("\x1b[?25l"))
#define tgfx_save_pos()           printf("\x1b[s")
#define tgfx_mv_savedpos()        printf("\x1b[u")
#define tgfx_disable_scrolling()  printf("\x1b[1;%dr", tgfx_w.ws_row+20)
#define tgfx_enable_scrolling()   printf("\x1b[r");

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
void tgfx_fb_put(int, int, char*);
void tgfx_fb_print(int, int, const char *);
void tgfx_fb_render();
void create_box(SPRITE*);

#define tgfx_fb_box() create_box(screenBuffer)


// Sprite Code
SPRITE *createSprite(int, int, int, int);
void sprite_fill_color(SPRITE*, char*, char*, char*);

#define sprite_fill(p, v) sprite_fill_color(p, v, WHITE, BLACK)

void sprite_blit(SPRITE *src, SPRITE *dst);

char *utf8string(uint32_t);

#endif // TERMINALGFX_H


