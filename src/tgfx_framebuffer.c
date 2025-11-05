#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "terminalgfx.h"

static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;

void tgfx_fb_init(int w, int h) {
  tgfx_width = w;
  tgfx_height = h;
  
  tgfx_buffer = malloc(h * sizeof(char *));
  for(int i = 0; i < h; i++){
    tgfx_buffer[i] = malloc(w * sizeof(char));
    memset(tgfx_buffer[i], ' ', w);
  }
}

void tgfx_fb_quit(){
  if(!tgfx_buffer) return;
  for(int i = 0; i < tgfx_height; i++)
    free(tgfx_buffer[i]);

  free(tgfx_buffer);
  tgfx_buffer = NULL;
}

void tgfx_fb_clear(char v){
  for(int i = 0; i < tgfx_height; i++) 
    memset(tgfx_buffer[i], v, tgfx_width);
}

void tgfx_fb_put(int x, int y, char c){
  // in case the coordinates are out of bounds
  if (x < 0 || x >= tgfx_width || y < 0 || y >= tgfx_height) 
    return;
  tgfx_buffer[y][x] = c;
}

void tgfx_fb_print(int x, int y, const char *s) {
    int i = 0;
    while (s[i] && (x + i) < tgfx_width) {
        tgfx_buffer[y][x + i] = s[i];
        i++;
    }
}

void tgfx_fb_box(){
  memset(tgfx_buffer[0], '-', tgfx_width);
  memset(tgfx_buffer[tgfx_height-1], '-', tgfx_width);
  for(int i = 1; i < tgfx_height - 1; i++){
    tgfx_buffer[i][0] = tgfx_buffer[i][tgfx_width-1] = '|'; 
  }
}

void tgfx_fb_render(){
  tgfx_mv_savedpos();
  tgfx_clfpos();
  tgfx_mv_savedpos();

  for (int i = 0; i < tgfx_height; i++){
    write(STDOUT_FILENO, tgfx_buffer[i], tgfx_width);
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
