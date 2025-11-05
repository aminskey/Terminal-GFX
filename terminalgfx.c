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

// Terminal settings and input variables
static struct termios tgfx_oldt;
static struct winsize tgfx_w;
static pthread_mutex_t inputLock;
static volatile int tgfx_key = 0;
static volatile int running = 0;

/*
*
*   INPUT BUFFER AND TERMINAL SETTINGS
*
*/
// predefining handle_sigint
static void handle_sigint(int sig);
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
  tgfx_cbreak();                              // start with cbreak and hide the cursor
  tgfx_toggleCursor(false);
}

struct winsize wsize(){return tgfx_w;}

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

/*
*
*   OUTPUT BUFFER AND SUPPORTING FUNCTIONS
*
*/
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
