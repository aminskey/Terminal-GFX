#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include "terminalgfx.h"

static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;

void tgfx_fb_init(int rows, int cols) {
  tgfx_width = cols;
  tgfx_height = rows;
  
  tgfx_buffer = malloc(rows * sizeof(char *));
  for(int i = 0; i < rows; i++){
    tgfx_buffer[i] = malloc(cols * sizeof(char));
    memset(tgfx_buffer[i], ' ', cols);
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

void tgfx_fb_render(){
  tgfx_mv_savedpos();
  tgfx_clfpos();
  tgfx_mv_savedpos();

  for (int i = 0; i < tgfx_height; i++){
    write(STDOUT_FILENO, tgfx_buffer[i], tgfx_width);
    putchar('\n');
  }
}
