#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "terminalgfx.h"

//static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;
static SPRITE *screenBuffer = NULL;

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

void tgfx_fb_quit(){
  if(!screenBuffer) return;
  for(int i = 0; i < tgfx_height; i++)
    free(screenBuffer->img[i]);

  free(screenBuffer->img);
  free(screenBuffer);
  screenBuffer = NULL;
}

void tgfx_fb_fill(char v){
  for(int i = 0; i < tgfx_height; i++) 
    memset(screenBuffer->img[i], v, tgfx_width);
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

void tgfx_fb_box(){
  memset(screenBuffer->img[0], '-', tgfx_width);
  memset(screenBuffer->img[tgfx_height-1], '-', tgfx_width);
  for(int i = 1; i < tgfx_height - 1; i++){
    screenBuffer->img[i][0] = screenBuffer->img[i][tgfx_width-1] = '|'; 
  }
  screenBuffer->img[0][0] = '+';
  screenBuffer->img[0][tgfx_width-1] = '+';

  screenBuffer->img[tgfx_height-1][0] = '+';
  screenBuffer->img[tgfx_height-1][tgfx_width-1] = '+';
}

void tgfx_fb_render(){
  tgfx_mv_savedpos();
  tgfx_clfpos();
  tgfx_mv_savedpos();

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
