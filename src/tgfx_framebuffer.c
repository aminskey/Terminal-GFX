#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "terminalgfx.h"

#define UTF8_MAX 5

//static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;
SPRITE *screenBuffer = NULL;

char WHITE[]={255, 255, 255};
char BLACK[]={0, 0, 0};
int tgfx_do_flush = 1;

static int utf8_length(unsigned char c) { 
  // we look at the MSB which is kept in the first 8 bits
  // The logic behind UTF-8 Characters is documented in section 3 of https://datatracker.ietf.org/doc/html/rfc3629
  if((c & 0x80) == 0x00) return 1;
  if((c & 0xE0) == 0xC0) return 2;
  if((c & 0xF0) == 0xE0) return 3;
  if((c & 0xF8) == 0xF0) return 4;
  return 1;
}

static void cell_set_glyph(CELL *c, const char *s, int len){
  if(len >= UTF8_MAX) len = UTF8_MAX - 1;
  memcpy(c->glyph, s, len);
  c->glyph[len] = '\0';
  c->glyph_len = len;
} 

SPRITE *createSprite(int x, int y, int w, int h) {
  SPRITE *spt = malloc(sizeof(SPRITE));
  spt->img = malloc(h * sizeof(CELL *));
  for(int i = 0; i < h; i++){
    spt->img[i] = malloc(w * sizeof(CELL));
    for(int j = 0; j < w; j++){
      cell_set_glyph(&spt->img[i][j], " ", 1);
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


void sprite_fill_color(SPRITE *p, char *v, char*fg, char*bg) {
  CELL exampleCell;
  cell_set_glyph(&exampleCell, v, utf8_length((unsigned char)v[0]));
  memcpy(exampleCell.fRGB, fg, 3);
  memcpy(exampleCell.bRGB, bg, 3);

  for(int i = 0; i < p->h; i++){
    for (int j = 0; j < p->w; j++)
      p->img[i][j] = exampleCell;
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

void tgfx_fb_put(int x, int y, char *c){
  // in case the coordinates are out of bounds
  if (x < 0 || x >= tgfx_width || y < 0 || y >= tgfx_height) 
    return;
  cell_set_glyph(&screenBuffer->img[y][x], c, utf8_length((unsigned char)c[0]));
}

void tgfx_fb_print(int x, int y, const char *s) { // refactor to strcpy 
    int i = 0;
    int cx = x;
    while (s[i] && cx < tgfx_width) {
        int len = utf8_length((unsigned char)s[i]);
        //memcpy(screenBuffer->img[y][cx].glyph, &s[i], len);
        //screenBuffer->img[y][cx].glyph[len+1] = '\0';

        cell_set_glyph(&screenBuffer->img[y][cx], &s[i], len);

        i += len;
        cx++;
    }
}

void create_box(SPRITE *p){
  char *vert = "│";
  char *hor = "─";

  char *c1 = "┌";
  char *c2 = "┐";
  char *c3 = "└";
  char *c4 = "┘";

  for(int k=0; k < p->w; k++){
    cell_set_glyph(&p->img[0][k], hor, utf8_length((unsigned char)hor[0]));
    cell_set_glyph(&p->img[p->h-1][k], hor, utf8_length((unsigned char)hor[0]));
  }

  for(int i = 1; i < p->h - 1; i++){
    cell_set_glyph(&p->img[i][0], vert, utf8_length((unsigned char)vert[0]));
    cell_set_glyph(&p->img[i][p->w-1], vert, utf8_length((unsigned char)vert[0]));
  }

  // Refactor to strcpy
  /* Adding corners */
  cell_set_glyph(&p->img[0][0], c1, utf8_length((unsigned char)c1[0]));
  cell_set_glyph(&p->img[0][p->w-1], c2, utf8_length((unsigned char)c2[0]));
  cell_set_glyph(&p->img[p->h-1][0], c3, utf8_length((unsigned char)c3[0]));
  cell_set_glyph(&p->img[p->h-1][p->w-1], c4, utf8_length((unsigned char)c4[0]));
}

void tgfx_fb_render(){
  //tgfx_move_cursor(screenBuffer->x, screenBuffer->y);

  for (int i = 0; i < tgfx_height; i++){
    for(int k = 0; k < tgfx_width; k++){
      unsigned char *fg = screenBuffer->img[i][k].fRGB;
      unsigned char *bg = screenBuffer->img[i][k].bRGB;

      printf("\x1b[38;2;%d;%d;%dm", fg[0], fg[1], fg[2]);
      printf("\x1b[48;2;%d;%d;%dm", bg[0], bg[1], bg[2]);
      printf("%s", screenBuffer->img[i][k].glyph);       // change to puts
      printf("\x1b[0m");
    }
    putchar('\n');
    if(tgfx_do_flush) fflush(stdout);
    /*
     *///tgfx_move_cursor(screenBuffer->x, screenBuffer->y+i);
  }
}
