#include <stdio.h>
#include <stdlib.h>
#include <terminalgfx.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/time.h>

double getTime(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1e6;
}

int main() {
  tgfx_init();
  tgfx_disable_scrolling();
  tgfx_startInput();

  tgfx_fb_init(tgfx_w.ws_col, tgfx_w.ws_row-4);
  if(!screenBuffer){
    perror("The screenbuffer could not initialize!!");
    return -1;
  }

  char blue[3] = {100, 100, 255};

  //getchar();

//  tgfx_fb_print(3, 4, (char*)utf8string(0x1f30d));
  int running=1;
  SPRITE *bx = createSprite(10, 10, 10, 10);
  sprite_fill_color(bx, ' ', TGFX_COL_WHITE, blue);
  create_box(bx); 

  double lastTime = getTime();
  double x, y;
  x = (double) bx->x;
  y = (double) bx->y;

  while(running){
    double currentTime = getTime();
    double dt = currentTime - lastTime;
    lastTime = currentTime;
    switch(tgfx_readInput(1)){
      case TGFX_KEY_UP:
        y-=y*dt;
        break;
      case TGFX_KEY_DOWN:
        y+=y*dt;
        break;
      case TGFX_KEY_LEFT:
        x-=x*dt;
        break;
      case TGFX_KEY_RIGHT:
        x+=x*dt;
        break;
      case '\x1b':
        running = 0;
        break;
      default:
        break;
    }

    bx->x = (int)x;
    bx->y = (int)y;

    // Drawing on screen buffer
    sprite_fill(screenBuffer, ' ');
    tgfx_fb_print(4, 4, "H␂ello, World");
    sprite_blit(bx, screenBuffer);
    tgfx_fb_box();
    // clearing and blitting screen buffer
    tgfx_move_cursor(0, 0);
    tgfx_fb_render();
    tgfx_tick(60);

  }
  tgfx_enable_scrolling();
  tgfx_terminate();
  return 0;
}
