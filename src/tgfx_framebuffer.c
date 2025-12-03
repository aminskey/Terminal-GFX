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

SPRITE *createSprite(int x, int y, int w, int h) {
  SPRITE *spt = malloc(sizeof(SPRITE));
  spt->img = malloc(h * sizeof(char *));
  for(int i = 0; i < h; i++){
    spt->img[i] = malloc(w * sizeof(char));
    memset(spt->img[i], ' ', w);
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
//void kill_sprite(){}

void tgfx_fb_quit(){
  if(!screenBuffer) return;
  for(int i = 0; i < tgfx_height; i++)
    free(screenBuffer->img[i]);

  free(screenBuffer->img);
  free(screenBuffer);
  screenBuffer = NULL;
}

void sprite_fill(SPRITE *p, char v) {
  for(int i = 0; i < p->h; i++) memset(p->img[i], v, p->w);
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
    memcpy(dest->img[i+py]+px, src->img[i+sy]+sx, w);
  }

}

void tgfx_fb_put(int x, int y, char c){
  // in case the coordinates are out of bounds
  if (x < 0 || x >= tgfx_width || y < 0 || y >= tgfx_height) 
    return;
  screenBuffer->img[y][x] = c;
}

void tgfx_fb_print(int x, int y, const char *s) {
    int i = 0;
    while (s[i] && (x + i) < tgfx_width) {
        screenBuffer->img[y][x + i] = s[i];
        i++;
    }
}

void create_box(SPRITE *p){
  memset(p->img[0], '-', p->w);
  memset(p->img[p->h-1], '-', p->w);
  for(int i = 1; i < p->h - 1; i++){
    p->img[i][0] = p->img[i][p->w-1] = '|'; 
  }

  /* Adding corners */
  p->img[0][0] = '+';
  p->img[0][p->w-1] = '+';
  p->img[p->h-1][0] = '+';
  p->img[p->h-1][p->w-1] = '+';
}

void tgfx_fb_render(){
  for (int i = 0; i < tgfx_height; i++){
    write(STDOUT_FILENO, screenBuffer->img[i], tgfx_width);
    putchar('\n');
  }
}

char *utf8string(uint32_t codepoint) {
    char *buf = malloc(5 * sizeof(char));

    if (codepoint <= 0x7F) {
        buf[0] = codepoint;
    } else if (codepoint <= 0x7FF) {
        buf[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
        buf[1] = 0x80 | (codepoint & 0x3F);
    } else if (codepoint <= 0xFFFF) {
        buf[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        buf[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        buf[2] = 0x80 | (codepoint & 0x3F);
    } else if (codepoint <= 0x10FFFF) {
        buf[0] = 0xF0 | ((codepoint >> 18) & 0x07);
        buf[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        buf[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        buf[3] = 0x80 | (codepoint & 0x3F);
    }

    buf[4]='\0';
    return buf;
}
