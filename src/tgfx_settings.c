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
struct winsize tgfx_w;

// predefining handle_sigint
static void handle_sigint(int sig);

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

void handle_sigint(int sig) {
  tgfx_terminate();
  tgfx_cls();
	printf("\nExiting due to signal: %d\n", sig);
	exit(EXIT_SUCCESS);
}

void tgfx_settings_init(){
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &tgfx_w);  // getting the window size
  tcgetattr(STDIN_FILENO, &tgfx_oldt);        // Saving the old terminal settings for later 
  signal(SIGINT, handle_sigint);              // Exit cleanly when sigerror
  tgfx_cbreak();                              // start with cbreak and hide the cursor
  tgfx_toggleCursor(false);
}
