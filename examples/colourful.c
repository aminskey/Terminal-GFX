#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <terminalgfx.h>
#include <math.h>

int main(void){
  tgfx_init();
  tgfx_startInput();

  char *msg = "═╣ Colourful Window! ╠═";
  int x = (80 - strlen("## colourful window! ##"))/2;

  // Don't flush the screen!
  //tgfx_do_flush = 0;

  tgfx_fb_init(80, 20);
  if(!screenBuffer){
    printf("ScreenBuffer could not be initialised!\n");
    return 0;
  }

  sprite_fill(screenBuffer, "▀");

  for(int i = 1; i < screenBuffer->h - 1; i++){
    for(int j = 1; j < screenBuffer->w - 1; j++){
      short *c1 = screenBuffer->img[i][j].bRGB;
      short *c2 = screenBuffer->img[i][j].fRGB;

      
      c1[0] = (char)(127.0 * sin(i)) + 255;//(char)rand()%255;
      c1[1] = (char)(127.0 * sin(j)) + 255;
      c1[2] = (char)(127.0 * sin(i)) + 255;
      
      c2[0] = (char)(127.0 * cos(j)) + 255; //(char)rand()%255;
      c2[1] = (char)(127.0 * cos(i)) + 255;//(char)rand()%255;
      c2[2] = (char)(127.0 * cos(j)) + 255;//(char)rand()%255;

      memcpy(screenBuffer->img[i][j].bRGB, c1, 3);
      memcpy(screenBuffer->img[i][j].fRGB, c2, 3);
    }
  }

  

  create_box(screenBuffer);
  tgfx_fb_print(x, 0, msg);
  tgfx_fb_render();
  while(tgfx_readInput(1) != 27){}
  tgfx_terminate();
  return 0;
}
