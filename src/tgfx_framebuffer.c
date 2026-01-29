#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "terminalgfx.h"

#define UTF8_MAX 5

#define boundUp(a,b)    (a > b)? b : a
#define boundLow(a,b)   (a < b)? b : a

//static char **tgfx_buffer = NULL;
static int tgfx_width = 0;
static int tgfx_height = 0;
SPRITE *screenBuffer = NULL;

short WHITE[3]={255, 255, 255};
short BLACK[3]={0, 0, 0};
short TRANS[3]={-1, -1, -1};
int tgfx_do_flush = 1;

static void colorBound(short *col, int low, int high){
  for(int i = 0; i < 3; i++){
    col[i] = boundLow(boundUp(col[i], high), low);
  }
}

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
} 

SPRITE *createSprite(int x, int y, int w, int h) {
  SPRITE *spt = malloc(sizeof(SPRITE));
  spt->img = malloc(h * sizeof(CELL *));
  for(int i = 0; i < h; i++){
    spt->img[i] = malloc(w * sizeof(CELL));
    for(int j = 0; j < w; j++){
      cell_set_glyph(&spt->img[i][j], " ", 1);
      memcpy(spt->img[i][j].fRGB, WHITE, 3*sizeof(short));
      memcpy(spt->img[i][j].bRGB, BLACK, 3*sizeof(short));
      //spt->img[i][j].trans = 0;
    }
  }

  spt->x = x;
  spt->y = y;
  spt->w = w;
  spt->h = h;

  return spt;
}

int colorCompare(short *a, short *b){
  int cnt = 0;
  for(int i = 0; i < 3; i++)
    if(a[i] != b[i]) cnt++;

  return cnt < 1;
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

  if(sp->img){
    for(int i = 0; i < sp->h ; i++){
      free(sp->img[i]);
      sp->img[i] = NULL;
    }
    free(sp->img);
    sp->img = NULL;
  }
  free(sp);
  *p = NULL;
}

int spriteCollide(SPRITE *self, SPRITE *other){
  return  (self->x + self->w >= other->x)   && 
          (self->x <= other->x + other->w)  &&
          (self->y + self->h >= other->y)   &&
          (self->y <= other->y + other->h); 
}

int groupCollide(SPRITE *self, SPRITE **group, int n){
  for (int i = 0; i < n; i++) {
    if(spriteCollide(self, group[i])) return 1;
  }
  return 0;
}


void tgfx_fb_quit(){
  kill_sprite(&screenBuffer);
}


void sprite_fill_color(SPRITE *p, char *v, short *fg, short *bg) {
  CELL exampleCell;
  cell_set_glyph(&exampleCell, v, utf8_length((unsigned char)v[0]));
  memcpy(exampleCell.fRGB, fg, 3*sizeof(short));
  memcpy(exampleCell.bRGB, bg, 3*sizeof(short));

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
  int tscore[2] = {0, 0};
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      if(colorCompare(src->img[i+sy][sx+j].fRGB, TRANS)){
        memcpy(src->img[i+sy][sx+j].fRGB, dest->img[i+py][px+j].fRGB, 3*sizeof(short));
        tscore[0] = 1;
      }

      if(colorCompare(src->img[i+sy][sx+j].bRGB, TRANS)){
        memcpy(src->img[i+sy][sx+j].bRGB, dest->img[i+py][px+j].bRGB, 3*sizeof(short));
        tscore[1] = 1;
      }

       // fix me later.
      dest->img[i+py][px+j] = src->img[i+sy][sx+j];

      // We only want to copy colours when blitting afterwards we revert
      
      if(tscore[0]) memcpy(src->img[i+sy][sx+j].fRGB, TRANS, 3*sizeof(short));
      if(tscore[1]) memcpy(src->img[i+sy][sx+j].bRGB, TRANS, 3*sizeof(short));
      
      tscore[0] = tscore[1] = 0;
    }
  }

}

void sprite_put(SPRITE *s, int x, int y, char *c){
  // in case the coordinates are out of bounds
  if (x < 0 || x >= s->w || y < 0 || y >= s->h) 
    return;
  cell_set_glyph(&s->img[y][x], c, utf8_length((unsigned char)c[0]));
}

void sprite_print(SPRITE *sp, int x, int y, const char *s) { // refactor to strcpy 
    int i = 0;
    int cx = x;
    while (s[i] && cx < sp->w) {
        int len = utf8_length((unsigned char)s[i]);
        //memcpy(screenBuffer->img[y][cx].glyph, &s[i], len);
        //screenBuffer->img[y][cx].glyph[len+1] = '\0';

        cell_set_glyph(&sp->img[y][cx], &s[i], len);

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
      short *fg = screenBuffer->img[i][k].fRGB;
      short *bg = screenBuffer->img[i][k].bRGB;

      colorBound(fg, 0, 255);
      colorBound(bg, 0, 255);

      //if(screenBuffer->img[i][k].alpha > 0){
      printf("\x1b[38;2;%d;%d;%dm", fg[0], fg[1], fg[2]);
      printf("\x1b[48;2;%d;%d;%dm", bg[0], bg[1], bg[2]);
      //}
      printf("%s", screenBuffer->img[i][k].glyph);       // change to puts
      printf("\x1b[0m");
    }
    putchar('\n');
    if(tgfx_do_flush) fflush(stdout);
    /*
     *///tgfx_move_cursor(screenBuffer->x, screenBuffer->y+i);
  }
}
