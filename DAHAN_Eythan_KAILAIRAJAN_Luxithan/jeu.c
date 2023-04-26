#include <ncurses.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "header.h"

void ncurses_initialiser() {
	initscr();
	cbreak();
	start_color();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	nodelay(stdscr, TRUE);
	refresh();
}

int main(){
	ncurses_initialiser();
  char mode = depart();
  switch(mode){
    case 'a':
      manuel();
      break;
    case 'b':
      automatique();
      break;
    default:

      break;
  }

  endwin();

  return 0;
}
