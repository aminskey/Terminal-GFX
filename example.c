#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminalgfx.h>

int main(void){
  tgfx_init();
  tgfx_startInput();

  // Don't flush the screen!
  //tgfx_do_flush = 0;

  tgfx_fb_init(80, 20);
  if(!screenBuffer){
    printf("ScreenBuffer could not be initialised!\n");
    return 0;
  }

  for(int i = 0; i < screenBuffer->h; i++){
    for(int j = 0; j < screenBuffer->w; j++){
      unsigned char *c = screenBuffer->img[i][j].bRGB;
      c[0] = (rand()%255);
      c[1] = (rand()%255);
      c[2] = (rand()%255);

      memcpy(screenBuffer->img[i][j].bRGB, c, 3);
    }
  }

  tgfx_fb_render();
  while(tgfx_readInput(1) != 27){}
  tgfx_terminate();
  return 0;
}
