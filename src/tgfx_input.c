#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>
#include<termios.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<stdlib.h>
#include<signal.h>
#include "terminalgfx.h"
#include<string.h>

static pthread_mutex_t inputLock;
static int tgfx_key = 0;
static int running = 0;

static void *inputThread(void *arg){
  while(running){
    int ch = getchar();
    pthread_mutex_lock(&inputLock);

    /* Processing arrow inputs */
    if(ch == 27) if(getchar() == '[') switch(getchar()){
      case 'A': ch = TGFX_KEY_UP;     break;
      case 'B': ch = TGFX_KEY_DOWN;   break;
      case 'C': ch = TGFX_KEY_RIGHT;  break;
      case 'D': ch = TGFX_KEY_LEFT;   break;
      default: break;
    }
    tgfx_key = ch;
    pthread_mutex_unlock(&inputLock);
  }
  return NULL;
}

void tgfx_input_init(){
    pthread_mutex_init(&inputLock, NULL);
}

void tgfx_input_terminate(){
    pthread_mutex_destroy(&inputLock);
}

void tgfx_startInput(){
  pthread_t t;
  running = 1;
  pthread_create(&t, NULL, inputThread, NULL);
  pthread_detach(t);
}

void tgfx_closeInput(){
  running = 0;
  usleep(10000);
}

int tgfx_readInput(int hold){
  int ch;
  pthread_mutex_lock(&inputLock);
  ch = tgfx_key;
  if(hold) tgfx_key = 0;
  pthread_mutex_unlock(&inputLock);
  return ch;
}
