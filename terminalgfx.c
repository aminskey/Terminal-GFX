#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>
#include<termios.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<stdlib.h>
#include<signal.h>
#include "terminalgfx.h"

static struct termios tgfx_oldt;
static struct winsize tgfx_w;
static pthread_mutex_t inputLock;
static volatile int tgfx_key = 0;
static volatile int running = 0;

static void handle_sigint(int sig);

static void *inputThread(void *arg){
  while(running){
    int ch = getchar();
    pthread_mutex_lock(&inputLock);
    if(ch == 27) if(getchar() == '[') switch((ch2 = getchar())){
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

void tgfx_cbreak(){
	struct termios trm;
	if(tcgetattr(STDIN_FILENO, &trm) < 0){
		perror("Cannot get attrs");
		exit(EXIT_FAILURE);
	}

	trm.c_lflag &= ~(ICANON | ECHO);
	trm.c_cc[VMIN] = 1;
	trm.c_cc[VTIME] = 0;
	
	if(tcsetattr(STDIN_FILENO, TCSANOW, &trm) < 0){
		perror("STDIN:");
		exit(EXIT_FAILURE);
	}
}

void tgfx_tick(int rate){
	usleep((int)(1000000/rate));
}

void tgfx_cls(){
	printf("\x1b[2J");
}

void tgfx_move_cursor(int y, int x) {
	printf("\x1b[%d;%dH", y, x);
}

void tgfx_toggleCursor(bool on){
	(on)? printf("\x1b[?25h") : printf("\x1b[?25l");
}

void tgfx_clfpos(){
	printf("\x1b[J");
}

void tgfx_save_pos(){
	printf("\x1b[s");
}

void tgfx_mv_savedpos(){
	printf("\x1b[u");
}

void tgfx_nocbreak(){
	tcsetattr(STDIN_FILENO, TCSANOW, &tgfx_oldt);
	tgfx_toggleCursor(true);
}

void tgfx_terminate(){
  tgfx_closeInput();
  pthread_mutex_destroy(&inputLock);
  tgfx_nocbreak();
}

void handle_sigint(int sig) {
	tgfx_terminate();
  tgfx_mv_savedpos();
  tgfx_clfpos();
	printf("\nExiting due to signal: %d\n", sig);
	exit(EXIT_SUCCESS);
}

void tgfx_init(){
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &tgfx_w);  // getting the window size
  tcgetattr(STDIN_FILENO, &tgfx_oldt);        // Saving the old terminal settings for later 
  signal(SIGINT, handle_sigint);              // Exit cleanly when sigerror
  pthread_mutex_init(&inputLock, NULL);
  tgfx_cbreak();                                   // start with cbreak and hide the cursor
  tgfx_toggleCursor(false);
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
