#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <terminalgfx.h>

extern const int LLAMA_FRAME_COUNT;
extern const int LLAMA_FRAME_WIDTH;
extern const int LLAMA_FRAME_HEIGHT;

extern const uint32_t llama_data[6][2304];

#define MAX_INDEX LLAMA_FRAME_WIDTH * LLAMA_FRAME_HEIGHT

void extract_rgb(uint32_t inp, unsigned char *col){
  col[1] = (inp >> 16) & 0xff;
  col[2] = (inp >> 8) & 0xff;
  col[0] = inp & 0xff;
}

void draw_frame(SPRITE *s, const uint32_t *frame){
  int y = 0;
  for(int i = 0; i < MAX_INDEX; i++){
    int x = i % LLAMA_FRAME_WIDTH;
    if(x == 0 && i != 0){ 
      y++; 
      i += LLAMA_FRAME_WIDTH;
    }

    CELL c = s->img[y][x];
    extract_rgb(frame[i], c.fRGB);

    if((i + 48) < MAX_INDEX){
      extract_rgb(frame[i + 48], c.bRGB);
    }
    s->img[y][x] = c;
  }
}

int main(void){
  tgfx_init();
  tgfx_startInput();

  char *msg = "═╣ Llama jumping ╠═";
  int x = (50 - strlen("## llama jumping ##"))/2;

  // Don't flush the screen!
  //tgfx_do_flush = 0;

  tgfx_fb_init(50, 26);
  if(!screenBuffer){
    printf("ScreenBuffer could not be initialised!\n");
    return 0;
  }

 
  SPRITE *animBox = createSprite(1, 1, LLAMA_FRAME_WIDTH, LLAMA_FRAME_HEIGHT-2);
  sprite_fill(animBox, "▀");
  int p = 0;

  tgfx_save_pos();
  while(tgfx_readInput(1) != 27){
    draw_frame(animBox, llama_data[p]);
    sprite_blit(animBox, screenBuffer);
    
    create_box(screenBuffer);
    tgfx_fb_print(x, 0, msg);

    p++;
    if(p >= LLAMA_FRAME_COUNT - 1) p = 0;

    tgfx_mv_savedpos();
    tgfx_fb_render();
    tgfx_tick(12);
  }
  
  tgfx_terminate();
  return 0;
}
