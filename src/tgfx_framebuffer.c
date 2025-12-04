#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "terminalgfx.h"

//static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;
SPRITE *screenBuffer = NULL;

char WHITE[]={255, 255, 255};
char BLACK[]={0, 0, 0};
int tgfx_do_flush = 1;

SPRITE *createSprite(int x, int y, int w, int h) {
  SPRITE *spt = malloc(sizeof(SPRITE));
  spt->img = malloc(h * sizeof(CELL *));
  for(int i = 0; i < h; i++){
    spt->img[i] = malloc(w * sizeof(CELL));
    for(int j = 0; j < w; j++){
      spt->img[i][j].c = ' ',
      memcpy(spt->img[i][j].fRGB, WHITE, 3);
      memcpy(spt->img[i][j].bRGB, BLACK, 3);
    }
  }

  spt->x = x;
  spt->y = y;
  spt->w = w;
  spt->h = h;

  return spt;
}

void tgfx_fb_init(int w, int h) {
  tgfx_width = w;
  tgfx_height = h;

  screenBuffer = createSprite(0, 0, w, h);
}

// Remember: Create a dynamic table for sprites!!
void kill_sprite(SPRITE **p){
  if(!*p) return;
  SPRITE *sp = *p;

  for(int i = 0; i < sp->h ; i++){
    free(sp->img[i]);
  }
  free(sp->img);
  free(sp);
  *p = NULL;
}

void tgfx_fb_quit(){
  kill_sprite(&screenBuffer);
}


void sprite_fill_color(SPRITE *p, char v, char*fg, char*bg) {
  for(int i = 0; i < p->h; i++){
    for(int j = 0; j < p->w; j++){
      p->img[i][j].c = v;
      memcpy(p->img[i][j].fRGB, fg, 3);
      memcpy(p->img[i][j].bRGB, bg, 3);
    }
  }
}

// Just like in pygame, when blitting an image onto another
// we use the destination's local coordinates and not the global ones!!
void sprite_blit(SPRITE *src, SPRITE*dest){

  // Under these circumstances, do not blit to dest!
  if(src->x + src->w < 0 || src->x > dest->w)
    return;
  
  if(src->y + src->h < 0 || src->y > dest->h)
    return;

 
  // Destination coordinates
  int px = (src->x < 0)? 0 : src->x;
  int py = (src->y < 0)? 0 : src->y;


  // Array indexing coordinates for src
  int sx = (src->x < 0)? -src->x : 0;
  int sy = (src->y < 0)? -src->y : 0;

  int w = src->w - sx;
  int h = src->h - sy;

  if(px + w > dest->w)
    w = dest->w - px;
  if (py + h > dest->h)
    h = dest->h - py;

  // Blit process
  for(int i = 0; i < h; i++){
    memcpy(dest->img[i+py]+px, src->img[i+sy]+sx, sizeof(CELL)*w);
  }

}

void tgfx_fb_put(int x, int y, char c){
  // in case the coordinates are out of bounds
  if (x < 0 || x >= tgfx_width || y < 0 || y >= tgfx_height) 
    return;
  screenBuffer->img[y][x].c = c;
}

void tgfx_fb_print(int x, int y, const char *s) {
    int i = 0;
    while (s[i] && (x + i) < tgfx_width) {
        screenBuffer->img[y][x + i].c = s[i];
        i++;
    }
}

void create_box(SPRITE *p){
  for(int k=0; k < p->w; k++)
    p->img[0][k].c = p->img[p->h-1][k].c = '-';
  
  for(int i = 1; i < p->h - 1; i++){
    p->img[i][0].c = p->img[i][p->w-1].c = '|'; 
  }

  /* Adding corners */
  p->img[0][0].c = 
  p->img[0][p->w-1].c =
  p->img[p->h-1][0].c = 
  p->img[p->h-1][p->w-1].c =
  '+';
}

void tgfx_fb_render(){
  //tgfx_move_cursor(screenBuffer->x, screenBuffer->y);

  for (int i = 0; i < tgfx_height; i++){
    for(int k = 0; k < tgfx_width; k++){
      unsigned char *fg = screenBuffer->img[i][k].fRGB;
      unsigned char *bg = screenBuffer->img[i][k].bRGB;

      printf("\x1b[38;2;%d;%d;%dm", fg[0], fg[1], fg[2]);
      printf("\x1b[48;2;%d;%d;%dm", bg[0], bg[1], bg[2]);
      putchar(screenBuffer->img[i][k].c);
      printf("\x1b[0m");
    }
    putchar('\n');
    if(tgfx_do_flush) fflush(stdout);
    /*
     *///tgfx_move_cursor(screenBuffer->x, screenBuffer->y+i);
  }
}
