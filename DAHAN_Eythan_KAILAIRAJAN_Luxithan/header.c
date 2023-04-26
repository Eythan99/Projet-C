#include <ncurses.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <locale.h>

char modele_N1[5][6];
char modele_S1[5][6];
char modele_O1[4][8];
char modele_E1[4][8];

char modele_N2[6][6];
char modele_S2[6][6];
char modele_O2[4][9];
char modele_E2[4][9];

typedef struct voiture VEHICULE;
struct voiture
{
  char direction ;          /*N => Nord, S => Sud, E => EST, O => OUEST*/
  int posx;                 /*Position courante coin haut gauche x de la voiture*/
  int posy;                 /*Position courante coin haut gauche y de la voiture*/
  int vitesse;              /*Vitesse du véhicule*/
  char alignement;          /*’g’=>gauche ou ’d’=>droite*/
  char type;                /*’v’=>voiture, ’c’=>camion, etc.*/
  char Carrosserie [4][30]; /*Carrosserie de la voiture, servira pourl’affichage du véhicule à tout moment*/
  int couleur;         /*Code couleur de la voiture à utiliser lors del’affichage*/
  char etat;                /*État du véhicule : ’1’ => actif et ’0’ => inactif*/
  int duree;
  int place;
  unsigned long int tps;    /*pour stocker le temps passé dans le parking*/
  struct voiture*NXT;       /*Pointeur vers une prochaine voiture,nécessaire pour la liste chaînée*/
};

void affichematrice(int nbLin,int nbCol, char **matri){
  int i, j;

  for (i = 0; i <nbLin; i++)
  {
    for (j =0; j <nbCol;j++)
    {
      if(matri[i][j]=='1') addch(ACS_HLINE);
			if(matri[i][j]=='2') addch(ACS_VLINE);
			if(matri[i][j]=='3') addch(ACS_TTEE);
			if(matri[i][j]=='4') addch(ACS_BTEE);
			if(matri[i][j]=='5') addch(ACS_RTEE);
			if(matri[i][j]=='6') addch(ACS_LTEE);
			if(matri[i][j]=='7') addch(ACS_ULCORNER);
			if(matri[i][j]=='8') addch(ACS_URCORNER);
			if(matri[i][j]=='9') addch(ACS_LRCORNER);
			if(matri[i][j]=='0') addch(ACS_LLCORNER);
			if(matri[i][j]=='\n') printw("\n");
			if(matri[i][j]==' ') printw(" ");
			if(matri[i][j]=='Q') printw(" ");
			if(matri[i][j]=='S') printw(" ");
			if(matri[i][j]=='G') printw(" ");
			if(matri[i][j]=='H') printw(" ");
      if(matri[i][j]=='I') printw(" ");
			if(matri[i][j]=='J') printw(" ");
			if(matri[i][j]=='g') printw(" ");
    }
    printw ("\n");
  }
}

char** init_tab2(int *nbLin, int *nbCol){
	FILE *f;
	char **table;
	char c;
	int i, j;
	*nbLin=55;
	//*nbCol = 177;
	char ch[800];

	f=fopen("Parking.txt","r");
    if(f==NULL)
    {
        printw("Le fichier en question est inexistant !\n");
				endwin();
        exit(-1);
    }

	fgets(ch,800,f);
	int x =ftell(f); //ftell retourne la position actuelle du curseur. Donc *nbCol = x
	//printw("\n%d\n", x);
	*nbCol = x;

    table = (char**)malloc(*nbLin * sizeof(char *));

    if (table == NULL)
    {
        printw("Echec d'allocation de place memoire\n");
				endwin();
        exit(-1);
    }
    for (i=0;i<*nbLin;i++)

    {
        table[i] = (char*)malloc(*nbCol * sizeof(char));

        if (table[i] == NULL)
        {
            printw("Echec d'allocation de place memoire\n");
						endwin();
            exit(-1);
        }

	}
  // Ecriture du tableau sur stdout
	fseek(f, 0, SEEK_SET);

   for (i=0;i<*nbLin;i++)
    {

      for(j=0;j<*nbCol;j++)
	      {
	      	c=fgetc(f);
	      	if((c !='\n'))
	            table[i][j]=c;

	      }
    }

	fclose(f);
	return table;
}

void printMatrixArray(int y, int x, int nbLin, int nbCol, char mat[nbLin][nbCol]) {
	int i, j;
	for (i = 0; i < nbLin; i++) {
		for (j = 0; j < nbCol; j++) {
      switch(mat[i][j]){
        case '0':
          mvaddch(x+i, y+j, ACS_VLINE);
          break;
        case '1':
          mvaddch(x+i, y+j, ACS_HLINE);
          break;
        case '2':
          mvaddch(x+i, y+j, ACS_LRCORNER);
          break;
        case '3':
          mvaddch(x+i, y+j, ACS_LLCORNER);
          break;
        case '4':
          mvaddch(x+i, y+j, ACS_URCORNER);
          break;
        case '5':
          mvaddch(x+i, y+j, ACS_ULCORNER);
          break;
        case '9':
          mvprintw(x+i, y+j, " ");
          break;
        default:
          mvprintw(x+i, y+j, " ");
          break;
      }
		}
		//printw("\n");
	}
}

void loadModels() {
	FILE *N1 = fopen("voiture1_marche_avant.txt", "r");
	FILE *E1 = fopen("voiture1_tournant_droit.txt", "r");
	FILE *S1 = fopen("voiture1_marche_arriere.txt", "r");
	FILE *O1 = fopen("voiture1_tournant_gauche.txt", "r");

	FILE *N2 = fopen("voiture2_marche_avant.txt", "r");
	FILE *E2 = fopen("voiture2_tournant_droit.txt", "r");
	FILE *S2 = fopen("voiture2_marche_arriere.txt", "r");
	FILE *O2 = fopen("voiture2_tournant_gauche.txt", "r");

	// if (N1 == NULL || E1 == NULL || S1 == NULL || O1 == NULL) exit(-1);
	// if (N2 == NULL || E2 == NULL || S2 == NULL || O2 == NULL) exit(-1);
	char c1, c2;
	int i = 0;
	int j = 0;

	while ((c1 = fgetc(N1)) != EOF) {
		c2 = fgetc(S1);
		if ((c1 >= '0' && c1 <= '9') || c1 == ' ') {
			modele_N1[i][j] = c1;
			modele_S1[i][j] = c2;
			j++;
			if (j == 6) {
				j = 0;
				i++;
			}
		}
	}
	i = 0;
	j = 0;
	while ((c1 = fgetc(E1)) != EOF) {
		c2 = fgetc(O1);
		if ((c1 >= '0' && c1 <= '9') || c1 == ' ') {
			modele_E1[i][j] = c1;
			modele_O1[i][j] = c2;
			j++;
			if (j == 8) {
				j = 0;
				i++;
			}
		}
	}
	i = 0;
	j = 0;
	while ((c1 = fgetc(N2)) != EOF) {
		c2 = fgetc(S2);
		if ((c1 >= '0' && c1 <= '9') || c1 == ' ') {
			modele_N2[i][j] = c1;
			modele_S2[i][j] = c2;
			j++;
			if (j == 6) {
				j = 0;
				i++;
			}
		}
	}
	i = 0;
	j = 0;
	while ((c1 = fgetc(E2)) != EOF) {
		c2 = fgetc(O2);
		if ((c1 >= '0' && c1 <= '9') || c1 == ' ') {
			modele_E2[i][j] = c1;
			modele_O2[i][j] = c2;
			j++;
			if (j == 9) {
				j = 0;
				i++;
			}
		}
	}
}

void printVehicule(VEHICULE *v) { // OK

  switch(v->type){
    case '1':
      switch(v->direction){
        case 'N':
        printMatrixArray(v->posx,v->posy,5, 5, modele_N1);
          break;
        case 'E':
        printMatrixArray(v->posx,v->posy,4, 7, modele_E1);
          break;
        case 'S':
        printMatrixArray(v->posx,v->posy,5, 5, modele_S1);
          break;
        case 'O':
        printMatrixArray(v->posx,v->posy,4, 7, modele_O1);
          break;
        default:
        printw("erreurd1\n");
          break;
      }
      break;
    case '2':
      switch(v->direction){
        case 'N':
        printMatrixArray(v->posx,v->posy,6, 5, modele_N2);
          break;
        case 'E':
        printMatrixArray(v->posx,v->posy,4, 8, modele_E2);
          break;
        case 'S':
        printMatrixArray(v->posx,v->posy,6, 5, modele_S2);
          break;
        case 'O':
        printMatrixArray(v->posx,v->posy,4, 8, modele_O2);
          break;
        default:
        printw("erreurd2\n");
          break;
      }
      break;
    default:
    printw("erreurt\n");
      break;
  }

}

VEHICULE*INIT_VEHICULE(){
	VEHICULE*v=malloc(sizeof(VEHICULE));

	v->posx=68;
	v->posy=50;
	v->vitesse=1;
      v->direction ='N';
      v->couleur=rand()%6+1;

	if (rand()%2 == 0) v->type = '1';
	else v->type = '2';

	return v;
}

void deplacementVoiture(char **matri, VEHICULE *v, char touche){
  switch(touche){
    case 'z':
      if(matri[v->posy][v->posx]!='1' && matri[v->posy][v->posx+1]!='1' && matri[v->posy][v->posx+2]!='1' && matri[v->posy][v->posx+3]!='1' && matri[v->posy][v->posx+4]!='1'){
        v->posy--;
        v->direction='N';
      }
      break;
    case 'q':
      if(matri[v->posy][v->posx-1]!='1' && matri[v->posy+1][v->posx-1]!='1' && matri[v->posy+2][v->posx-1]!='1'
			 && matri[v->posy][v->posx-1]!='2' && matri[v->posy+1][v->posx-1]!='2' && matri[v->posy+2][v->posx-1]!='2'){
        v->posx--;
        v->direction='O';
      }
      break;
    case 's':
			if(v->direction=='E'){
				if(matri[v->posy][v->posx+3]!='1' && matri[v->posy+1][v->posx+3]!='1' && matri[v->posy+2][v->posx+3]!='1' && matri[v->posy+3][v->posx+3]!='1' && matri[v->posy+4][v->posx+3]!='1'
			&& matri[v->posy][v->posx+3]!='2' && matri[v->posy+1][v->posx+3]!='2' && matri[v->posy+2][v->posx+3]!='2' && matri[v->posy+3][v->posx+3]!='2' && matri[v->posy+4][v->posx+3]!='2'
			 && matri[v->posy][v->posx+4]!='1' && matri[v->posy+1][v->posx+4]!='1' && matri[v->posy+2][v->posx+4]!='1' && matri[v->posy+3][v->posx+4]!='1' && matri[v->posy+4][v->posx+4]!='1'
		 && matri[v->posy][v->posx+4]!='2' && matri[v->posy+1][v->posx+4]!='2' && matri[v->posy+2][v->posx+4]!='2' && matri[v->posy+3][v->posx+4]!='2' && matri[v->posy+4][v->posx+4]!='2'){
					v->direction='S';
				}
			}else	if(v->direction=='O'){
					if(matri[v->posy][v->posx]!='1' && matri[v->posy+1][v->posx]!='1' && matri[v->posy+2][v->posx]!='1' && matri[v->posy+3][v->posx]!='1' && matri[v->posy+4][v->posx]!='1'
				&& matri[v->posy][v->posx]!='2' && matri[v->posy+1][v->posx]!='2' && matri[v->posy+2][v->posx]!='2' && matri[v->posy+3][v->posx]!='2' && matri[v->posy+4][v->posx]!='2'
				 && matri[v->posy][v->posx+1]!='1' && matri[v->posy+1][v->posx+1]!='1' && matri[v->posy+2][v->posx+1]!='1' && matri[v->posy+3][v->posx+1]!='1' && matri[v->posy+4][v->posx+1]!='1'
			 && matri[v->posy][v->posx+1]!='2' && matri[v->posy+1][v->posx+1]!='2' && matri[v->posy+2][v->posx+1]!='2' && matri[v->posy+3][v->posx+1]!='2' && matri[v->posy+4][v->posx+1]!='2'){
						v->direction='S';
					}
				}else if(matri[v->posy+4][v->posx]!='1' && matri[v->posy+4][v->posx+1]!='1' && matri[v->posy+4][v->posx+2]!='1' && matri[v->posy+4][v->posx+3]!='1' && matri[v->posy+4][v->posx+4]!='1'){
        v->posy++;
        v->direction='S';
      }
      break;
    case 'd':
      if(matri[v->posy][v->posx+8]!='1' && matri[v->posy+1][v->posx+8]!='1' && matri[v->posy+2][v->posx+8]!='1'
			 && matri[v->posy][v->posx+8]!='2' && matri[v->posy+1][v->posx+8]!='2' && matri[v->posy+2][v->posx+8]!='2'){
        v->posx++;
        v->direction='E';
      }
      break;
    default:

      break;
  }
}

char key_pressed() {
	struct termios oldterm, newterm;
	int oldfd;
	char c, result = 0;
	tcgetattr (STDIN_FILENO, &oldterm);
	newterm = oldterm;
	newterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newterm);
	oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl (STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
	c = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldterm);
	fcntl (STDIN_FILENO, F_SETFL, oldfd);
	if (c != EOF) {
		ungetc(c, stdin);
		result = getchar();
	}
	return result;
}


void introduction(){
	char touche = '*';
        char c;
        FILE * fic = fopen("presentation.txt", "r");
        while ((c=fgetc(fic)) != EOF){
                printw("%c", c);
        }
        fclose(fic);
	
	printw("\n\n");

	FILE * ficc = fopen("espace.txt", "r");
        while ((c=fgetc(ficc)) != EOF){
                printw("%c", c);
        }
        fclose(ficc);

	refresh();
	while (touche != ' '){
		touche = key_pressed();
	}
}

char menu(){

	clear();
char touche = '*';
char c;
        FILE * fic = fopen("menu.txt", "r");
        while ((c=fgetc(fic)) != EOF){
                printw("%c", c);
        }
        fclose(fic);
refresh();
while (touche != 'a' && touche != 'b'){
	touche = key_pressed();
}
return touche;
}

char depart(){

	move(0,0);
introduction();

return menu();
}

void initColor(){

  init_color(1,999,999,999);//blanc
	init_color(2,999,000,000);//rouge
	init_color(3,000,700,000);//vert
	init_color(4,000,000,999);//bleu
	init_color(5,999,999,0);//jaune
	init_color(6,999,500,000);//Orange

	init_pair(1, 1, 0);
  init_pair(2, 2, 0);
  init_pair(3, 3, 0);
  init_pair(4, 4, 0);
  init_pair(5, 5, 0);
  init_pair(6, 6, 0);

	attron(COLOR_PAIR(1));
}

void affPlace(){

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(12,0,"X");
        mvprintw(13,0,"X");
        mvprintw(14,0,"X");
        mvprintw(15,0,"X");
        mvprintw(16,0,"X");
        mvprintw(17,0,"X");
        mvprintw(18,0,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(20,0,"X");
        mvprintw(21,0,"X");
        mvprintw(22,0,"X");
        mvprintw(23,0,"X");
        mvprintw(24,0,"X");
        mvprintw(25,0,"X");
        mvprintw(26,0,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(28,0,"X");
        mvprintw(29,0,"X");
        mvprintw(30,0,"X");
        mvprintw(31,0,"X");
        mvprintw(32,0,"X");
        mvprintw(33,0,"X");
        mvprintw(34,0,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(36,0,"X");
        mvprintw(37,0,"X");
        mvprintw(38,0,"X");
        mvprintw(39,0,"X");
        mvprintw(40,0,"X");
        mvprintw(41,0,"X");
        mvprintw(42,0,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(12,46,"X");
        mvprintw(13,46,"X");
        mvprintw(14,46,"X");
        mvprintw(15,46,"X");
        mvprintw(16,46,"X");
        mvprintw(17,46,"X");
        mvprintw(18,46,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(20,46,"X");
        mvprintw(21,46,"X");
        mvprintw(22,46,"X");
        mvprintw(23,46,"X");
        mvprintw(24,46,"X");
        mvprintw(25,46,"X");
        mvprintw(26,46,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(28,46,"X");
        mvprintw(29,46,"X");
        mvprintw(30,46,"X");
        mvprintw(31,46,"X");
        mvprintw(32,46,"X");
        mvprintw(33,46,"X");
        mvprintw(34,46,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(36,46,"X");
        mvprintw(37,46,"X");
        mvprintw(38,46,"X");
        mvprintw(39,46,"X");
        mvprintw(40,46,"X");
        mvprintw(41,46,"X");
        mvprintw(42,46,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(12,47,"X");
        mvprintw(13,47,"X");
        mvprintw(14,47,"X");
        mvprintw(15,47,"X");
        mvprintw(16,47,"X");
        mvprintw(17,47,"X");
        mvprintw(18,47,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(20,47,"X");
        mvprintw(21,47,"X");
        mvprintw(22,47,"X");
        mvprintw(23,47,"X");
        mvprintw(24,47,"X");
        mvprintw(25,47,"X");
        mvprintw(26,47,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(28,47,"X");
        mvprintw(29,47,"X");
        mvprintw(30,47,"X");
        mvprintw(31,47,"X");
        mvprintw(32,47,"X");
        mvprintw(33,47,"X");
        mvprintw(34,47,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(36,47,"X");
        mvprintw(37,47,"X");
        mvprintw(38,47,"X");
        mvprintw(39,47,"X");
        mvprintw(40,47,"X");
        mvprintw(41,47,"X");
        mvprintw(42,47,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(12,93,"X");
        mvprintw(13,93,"X");
        mvprintw(14,93,"X");
        mvprintw(15,93,"X");
        mvprintw(16,93,"X");
        mvprintw(17,93,"X");
        mvprintw(18,93,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(20,93,"X");
        mvprintw(21,93,"X");
        mvprintw(22,93,"X");
        mvprintw(23,93,"X");
        mvprintw(24,93,"X");
        mvprintw(25,93,"X");
        mvprintw(26,93,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(28,93,"X");
        mvprintw(29,93,"X");
        mvprintw(30,93,"X");
        mvprintw(31,93,"X");
        mvprintw(32,93,"X");
        mvprintw(33,93,"X");
        mvprintw(34,93,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(3));
        mvprintw(36,93,"X");
        mvprintw(37,93,"X");
        mvprintw(38,93,"X");
        mvprintw(39,93,"X");
        mvprintw(40,93,"X");
        mvprintw(41,93,"X");
        mvprintw(42,93,"X");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));

}

void affichageMessage2(){


mvprintw(10, 100,"                                                             _             _                   _ ");
mvprintw(11, 100,"   __ _ _ __  _ __  _   _ _   _  ___ _ __   ___ _   _ _ __  | | ___  ___  | |_ ___  _   _  ___| |__   ___  ___ ");
mvprintw(12, 100,"  / _` | '_ \\| '_ \\| | | | | | |/ _ \\ '__| / __| | | | '__| | |/ _ \\/ __| | __/ _ \\| | | |/ __| '_ \\ / _ \\/ __|");
mvprintw(13, 100," | (_| | |_) | |_) | |_| | |_| |  __/ |    \\__ \\ |_| | |    | |  __/\\__ \\ | || (_) | |_| | (__| | | |  __/\\__ \\");
mvprintw(14, 100,"  \\__,_| .__/| .__/ \\__,_|\\__, |\\___|_|    |___/\\__,_|_|    |_|\\___||___/  \\__\\___/ \\__,_|\\___|_| |_|\\___||___/");
mvprintw(15, 100,"       |_| _ |_| _ _      |___/    _ _     _ _                            _                                    ");
mvprintw(16, 100,"          ( )___( | ) __ _( | )___( | ) __| ( )  _ __   ___  _   _ _ __  | |__   ___  _   _  __ _  ___ _ __    ");
mvprintw(17, 100,"          |/|_  //|/ / _` |/|// __|/|/ / _` |/  | '_ \\ / _ \\| | | | '__| | '_ \\ / _ \\| | | |/ _` |/ _ \\ '__|   ");
mvprintw(18, 100,"             / /    | (_| |   \\__ \\   | (_| |   | |_) | (_) | |_| | |    | |_) | (_) | |_| | (_| |  __/ |      ");
mvprintw(19, 100,"            /___|    \\__, |   |___/    \\__,_|   | .__/ \\___/ \\__,_|_|    |_.__/ \\___/ \\__,_|\\__, |\\___|_|      ");
mvprintw(20, 100,"                        |_|                     |_|                                         |___/              ");


}

void manuel(){
	setlocale(LC_ALL, "");
  initColor();
	clear();
	refresh();

  int cbreak();
  srand(time(NULL));

  int nbLin, nbCol;
	char ** mat;

  mat=init_tab2(&nbLin, &nbCol);

  loadModels();
  VEHICULE *v = INIT_VEHICULE();
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(v->couleur));
  printVehicule(v);
  attroff(COLOR_PAIR(v->couleur));
  attron(COLOR_PAIR(1));

	int montant=0;

  char touche='e';
  while(1){
		if(v->posy<51 || touche!='s'){
    deplacementVoiture(mat,v, touche);
		}
		erase();
    affichematrice(nbLin,nbCol, mat);
    affichageMessage2();
		//mvprintw(0,0,"%d %d %d", v->posy,  v->posx, montant);
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(v->couleur));
    printVehicule(v);
    attroff(COLOR_PAIR(v->couleur));
    attron(COLOR_PAIR(1));
		//mvprintw(v->posy, v->posx, "f");
    affPlace();
		if(mat[v->posy][v->posx]=='g'){
			switch(v->posx){
				case 1 ... 16:
				switch(v->posy){
					case 11 ... 18:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(12,0,"X");
						mvprintw(13,0,"X");
						mvprintw(14,0,"X");
						mvprintw(15,0,"X");
						mvprintw(16,0,"X");
						mvprintw(17,0,"X");
						mvprintw(18,0,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 19 ... 26:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(20,0,"X");
						mvprintw(21,0,"X");
						mvprintw(22,0,"X");
						mvprintw(23,0,"X");
						mvprintw(24,0,"X");
						mvprintw(25,0,"X");
						mvprintw(26,0,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 27 ... 34:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(28,0,"X");
						mvprintw(29,0,"X");
						mvprintw(30,0,"X");
						mvprintw(31,0,"X");
						mvprintw(32,0,"X");
						mvprintw(33,0,"X");
						mvprintw(34,0,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 35 ... 42:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(36,0,"X");
						mvprintw(37,0,"X");
						mvprintw(38,0,"X");
						mvprintw(39,0,"X");
						mvprintw(40,0,"X");
						mvprintw(41,0,"X");
						mvprintw(42,0,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
				}
					break;
				case 30 ... 44:
					switch(v->posy){
					case 11 ... 18:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(12,46,"X");
						mvprintw(13,46,"X");
						mvprintw(14,46,"X");
						mvprintw(15,46,"X");
						mvprintw(16,46,"X");
						mvprintw(17,46,"X");
						mvprintw(18,46,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 19 ... 26:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(20,46,"X");
						mvprintw(21,46,"X");
						mvprintw(22,46,"X");
						mvprintw(23,46,"X");
						mvprintw(24,46,"X");
						mvprintw(25,46,"X");
						mvprintw(26,46,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 27 ... 34:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(28,46,"X");
						mvprintw(29,46,"X");
						mvprintw(30,46,"X");
						mvprintw(31,46,"X");
						mvprintw(32,46,"X");
						mvprintw(33,46,"X");
						mvprintw(34,46,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 35 ... 42:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(36,46,"X");
						mvprintw(37,46,"X");
						mvprintw(38,46,"X");
						mvprintw(39,46,"X");
						mvprintw(40,46,"X");
						mvprintw(41,46,"X");
						mvprintw(42,46,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
				}
					break;
				case 48 ... 63:
					switch(v->posy){
					case 11 ... 18:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(12,47,"X");
						mvprintw(13,47,"X");
						mvprintw(14,47,"X");
						mvprintw(15,47,"X");
						mvprintw(16,47,"X");
						mvprintw(17,47,"X");
						mvprintw(18,47,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 19 ... 26:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(20,47,"X");
						mvprintw(21,47,"X");
						mvprintw(22,47,"X");
						mvprintw(23,47,"X");
						mvprintw(24,47,"X");
						mvprintw(25,47,"X");
						mvprintw(26,47,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 27 ... 34:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(28,47,"X");
						mvprintw(29,47,"X");
						mvprintw(30,47,"X");
						mvprintw(31,47,"X");
						mvprintw(32,47,"X");
						mvprintw(33,47,"X");
						mvprintw(34,47,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 35 ... 42:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(36,47,"X");
						mvprintw(37,47,"X");
						mvprintw(38,47,"X");
						mvprintw(39,47,"X");
						mvprintw(40,47,"X");
						mvprintw(41,47,"X");
						mvprintw(42,47,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
				}
					break;
				case 77 ... 87:
					switch(v->posy){
					case 11 ... 18:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(12,93,"X");
						mvprintw(13,93,"X");
						mvprintw(14,93,"X");
						mvprintw(15,93,"X");
						mvprintw(16,93,"X");
						mvprintw(17,93,"X");
						mvprintw(18,93,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 19 ... 26:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(20,93,"X");
						mvprintw(21,93,"X");
						mvprintw(22,93,"X");
						mvprintw(23,93,"X");
						mvprintw(24,93,"X");
						mvprintw(25,93,"X");
						mvprintw(26,93,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 27 ... 34:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(28,93,"X");
						mvprintw(29,93,"X");
						mvprintw(30,93,"X");
						mvprintw(31,93,"X");
						mvprintw(32,93,"X");
						mvprintw(33,93,"X");
						mvprintw(34,93,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
					case 35 ... 42:
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(2));
						mvprintw(36,93,"X");
						mvprintw(37,93,"X");
						mvprintw(38,93,"X");
						mvprintw(39,93,"X");
						mvprintw(40,93,"X");
						mvprintw(41,93,"X");
						mvprintw(42,93,"X");
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
						break;
				}
					break;
				default:

					break;
			}
			montant ++;
		}
		refresh();
    touche=key_pressed();
		if((v->posy<51 && v->posy>49 && v->posx<25 && v->posx>17 && v->direction=='S') || touche=='a'){
			break;
		}
  }
	erase();
  affichematrice(nbLin,nbCol, mat);
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(v->couleur));
  printVehicule(v);
  attroff(COLOR_PAIR(v->couleur));
  attron(COLOR_PAIR(1));
	montant/=100;
	int montant2= montant%100;
	montant=(montant-montant2)/100;
	clear();
	mvprintw(55,10," le montant que vous devez payer est : %d$%d", montant, montant2);
	move(0,0);
	char c;
        FILE * fic = fopen("fin.txt", "r");
        while ((c=fgetc(fic)) != EOF){
                printw("%c", c);
        }
        fclose(fic);

        printw("\n\n");

        FILE * ficc = fopen("espace.txt", "r");
        while ((c=fgetc(ficc)) != EOF){
                printw("%c", c);
        }
        fclose(ficc);

	refresh();
	touche ='r';
	while(touche!=' '){
		touche=key_pressed();
	}
}

void ajoutVoit(VEHICULE * voiture1){
	VEHICULE * voitCourant = voiture1;
	while (voitCourant->NXT != NULL) {
			voitCourant = voitCourant->NXT;
	}
	voitCourant->NXT = malloc(sizeof(VEHICULE));
	voitCourant->NXT->posx = 68;
	voitCourant->NXT->posy = 50;
	voitCourant->NXT->direction ='N';
	voitCourant->NXT->vitesse =1;
  voitCourant->NXT->tps = 0;
  voitCourant->NXT->etat = 'D';
  voitCourant->NXT->duree=rand()%100+30;
  voitCourant->NXT->couleur = rand()%6+1;;
	if (rand()%2 == 0) voitCourant->NXT->type = '1';
	else voitCourant->NXT->type = '2';
	voitCourant->NXT->NXT = NULL;

}

void deplacementVoiture2(char **matri, VEHICULE *v, int place[16],  VEHICULE *v1){

   VEHICULE *listeVoit = v1;
   int colision;

	char touche='a';
  if(v->etat=='G'){
    switch(v->direction){
      case 'E':
        v->tps++;
        if(v->tps<15){
          v->posx++;
        }else if(v->tps>=v->duree && v->tps<=v->duree+12){
          colision=0;
          while(listeVoit!=NULL){
            if(listeVoit->posx>v->posx-20 && listeVoit->posx<v->posx && listeVoit->posy>v->posy-10 && listeVoit->posy<v->posy+10){
                colision=1;
            }
            listeVoit=listeVoit->NXT;
          }
          if(colision==0){
            v->posx--;
          }else{
            v->tps--;
          }
        }else if(v->tps==v->duree+13){
          v->etat='N';
          place[v->place]=0;
          if(v->posx>50){
            v->direction='N';
          }else{
            v->direction='S';
          }
        }
        break;
      case 'O':
        v->tps++;
        if(v->tps<15){
          v->posx--;
        }else if(v->tps>=v->duree && v->tps<=v->duree+12){
          colision=0;
          while(listeVoit!=NULL){
            if(listeVoit->posx>v->posx&& listeVoit->posx<v->posx+27  && listeVoit->posy>v->posy-10 && listeVoit->posy<v->posy+10){
              if(listeVoit->etat!='G'){
                colision=1;
              }
            }
            listeVoit=listeVoit->NXT;
          }
          if(colision==0){
            v->posx++;
          }else{
            v->tps--;
          }
          //v->posx++;
        }else if(v->tps==v->duree+13){
          v->etat='N';
          place[v->place]=0;
          if(v->posx>50){
            v->direction='N';
          }else{
            v->direction='S';
          }
        }
        break;
      default:

        break;
    }
  }else{

  	switch(matri[v->posy][v->posx]){
  		case 'Q':
  			touche='q';
  			break;
  		case 'S':
  			touche='s';
  			break;
      case 'G':
        if(v->etat=='D'){
          switch(v->direction){
          case 'N':
            if(place[0]==0){
              place[0]=1;
              v->etat='G';
              v->direction='O';
              v->place=0;
            }else if(place[1]==0){
              place[1]=1;
              v->etat='G';
              v->direction='E';
              v->place=1;
            }else{
              touche='z';
            }
            break;
          case 'S':
            if(place[8]==0){
              place[8]=1;
              v->etat='G';
              v->direction='O';
              v->place=8;
            }else if(place[9]==0){
              place[9]=1;
              v->etat='G';
              v->direction='E';
              v->place=9;
            }else{
              touche='s';
            }
            break;
          default:

            break;
        }
        }else{
          switch(v->direction){
    				case 'O':
    					touche = 'q';
    					break;
    				case 'E':
    					touche = 'd';
    					break;
    				case 'N':
    					touche = 'z';
    					break;
    				case 'S':
    					touche = 's';
    					break;
    				default:
    					break;
    			}
        }
        break;
      case 'H':
        if(v->etat=='D'){
          switch(v->direction){
        case 'N':
          if(place[2]==0){
            place[2]=1;
            v->etat='G';
            v->direction='O';
            v->place=2;
          }else if(place[3]==0){
            place[3]=1;
            v->etat='G';
            v->direction='E';
            v->place=3;
          }else{
            touche='z';
          }
          break;
        case 'S':
          if(place[10]==0){
            place[10]=1;
            v->etat='G';
            v->direction='O';
            v->place=10;
          }else if(place[11]==0){
            place[11]=1;
            v->etat='G';
            v->direction='E';
            v->place=11;
          }else{
            touche='s';
          }
          break;
        default:

          break;
      }
        }else{
          switch(v->direction){
    				case 'O':
    					touche = 'q';
    					break;
    				case 'E':
    					touche = 'd';
    					break;
    				case 'N':
    					touche = 'z';
    					break;
    				case 'S':
    					touche = 's';
    					break;
    				default:
    					break;
    			}
        }
        break;
      case 'I':
        if(v->etat=='D'){
          switch(v->direction){
        case 'N':
          if(place[4]==0){
            place[4]=1;
            v->etat='G';
            v->direction='O';
            v->place=4;
          }else if(place[5]==0){
            place[5]=1;
            v->etat='G';
            v->direction='E';
            v->place=5;
          }else{
            touche='z';
          }
          break;
        case 'S':
          if(place[12]==0){
            place[12]=1;
            v->etat='G';
            v->direction='O';
            v->place=12;
          }else if(place[13]==0){
            place[13]=1;
            v->etat='G';
            v->direction='E';
            v->place=13;
          }else{
            touche='s';
          }
          break;
        default:

          break;
      }
        }else{
          switch(v->direction){
    				case 'O':
    					touche = 'q';
    					break;
    				case 'E':
    					touche = 'd';
    					break;
    				case 'N':
    					touche = 'z';
    					break;
    				case 'S':
    					touche = 's';
    					break;
    				default:
    					break;
    			}
        }
        break;
      case 'J':
        if(v->etat=='D'){
          switch(v->direction){
        case 'N':
          if(place[6]==0){
            place[6]=1;
            v->etat='G';
            v->direction='O';
            v->place=6;
          }else if(place[7]==0){
            place[7]=1;
            v->etat='G';
            v->direction='E';
            v->place=7;
          }else{
            touche='z';
          }
          break;
        case 'S':
          if(place[14]==0){
            place[14]=1;
            v->etat='G';
            v->direction='O';
            v->place=14;
          }else if(place[15]==0){
            place[15]=1;
            v->etat='G';
            v->direction='E';
            v->place=15;
          }else{
            touche='s';
          }
          break;
        default:

          break;
      }
        }else{
          switch(v->direction){
    				case 'O':
    					touche = 'q';
    					break;
    				case 'E':
    					touche = 'd';
    					break;
    				case 'N':
    					touche = 'z';
    					break;
    				case 'S':
    					touche = 's';
    					break;
    				default:
    					break;
    			}
        }
        break;
  		default:
  			switch(v->direction){
  				case 'O':
  					touche = 'q';
  					break;
  				case 'E':
  					touche = 'd';
  					break;
  				case 'N':
  					touche = 'z';
  					break;
  				case 'S':
  					touche = 's';
  					break;
  				default:
  					break;
  			}
  			break;
  	}

    switch(touche){
      case 'z':
        if(matri[v->posy][v->posx]!='1' && matri[v->posy][v->posx+1]!='1' && matri[v->posy][v->posx+2]!='1' && matri[v->posy][v->posx+3]!='1' && matri[v->posy][v->posx+4]!='1'){
          v->posy--;
          v->direction='N';
        }
        break;
      case 'q':
        if(matri[v->posy][v->posx-1]!='1' && matri[v->posy+1][v->posx-1]!='1' && matri[v->posy+2][v->posx-1]!='1'
  			 && matri[v->posy][v->posx-1]!='2' && matri[v->posy+1][v->posx-1]!='2' && matri[v->posy+2][v->posx-1]!='2'){
          v->posx--;
          v->direction='O';
        }
        break;
      case 's':
  			if(v->direction=='E'){
  				if(matri[v->posy][v->posx+3]!='1' && matri[v->posy+1][v->posx+3]!='1' && matri[v->posy+2][v->posx+3]!='1' && matri[v->posy+3][v->posx+3]!='1' && matri[v->posy+4][v->posx+3]!='1'
  			&& matri[v->posy][v->posx+3]!='2' && matri[v->posy+1][v->posx+3]!='2' && matri[v->posy+2][v->posx+3]!='2' && matri[v->posy+3][v->posx+3]!='2' && matri[v->posy+4][v->posx+3]!='2'
  			 && matri[v->posy][v->posx+4]!='1' && matri[v->posy+1][v->posx+4]!='1' && matri[v->posy+2][v->posx+4]!='1' && matri[v->posy+3][v->posx+4]!='1' && matri[v->posy+4][v->posx+4]!='1'
  		 && matri[v->posy][v->posx+4]!='2' && matri[v->posy+1][v->posx+4]!='2' && matri[v->posy+2][v->posx+4]!='2' && matri[v->posy+3][v->posx+4]!='2' && matri[v->posy+4][v->posx+4]!='2'){
  					v->direction='S';
  				}
  			}else	if(v->direction=='O'){
  					if(matri[v->posy][v->posx]!='1' && matri[v->posy+1][v->posx]!='1' && matri[v->posy+2][v->posx]!='1' && matri[v->posy+3][v->posx]!='1' && matri[v->posy+4][v->posx]!='1'
  				&& matri[v->posy][v->posx]!='2' && matri[v->posy+1][v->posx]!='2' && matri[v->posy+2][v->posx]!='2' && matri[v->posy+3][v->posx]!='2' && matri[v->posy+4][v->posx]!='2'
  				 && matri[v->posy][v->posx+1]!='1' && matri[v->posy+1][v->posx+1]!='1' && matri[v->posy+2][v->posx+1]!='1' && matri[v->posy+3][v->posx+1]!='1' && matri[v->posy+4][v->posx+1]!='1'
  			 && matri[v->posy][v->posx+1]!='2' && matri[v->posy+1][v->posx+1]!='2' && matri[v->posy+2][v->posx+1]!='2' && matri[v->posy+3][v->posx+1]!='2' && matri[v->posy+4][v->posx+1]!='2'){
  						v->direction='S';
  					}
  				}else if(matri[v->posy+4][v->posx]!='1' && matri[v->posy+4][v->posx+1]!='1' && matri[v->posy+4][v->posx+2]!='1' && matri[v->posy+4][v->posx+3]!='1' && matri[v->posy+4][v->posx+4]!='1'){
          v->posy++;
          v->direction='S';
        }
        break;
      case 'd':
        if(matri[v->posy][v->posx+8]!='1' && matri[v->posy+1][v->posx+8]!='1' && matri[v->posy+2][v->posx+8]!='1'
  			 && matri[v->posy][v->posx+8]!='2' && matri[v->posy+1][v->posx+8]!='2' && matri[v->posy+2][v->posx+8]!='2'){
          v->posx++;
          v->direction='E';
        }
        break;
      default:

        break;
    }
  }
}

void affichePlace(int place[12]){

      if(place[8]==1){
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));
        mvprintw(12,0,"X");
        mvprintw(13,0,"X");
        mvprintw(14,0,"X");
        mvprintw(15,0,"X");
        mvprintw(16,0,"X");
        mvprintw(17,0,"X");
        mvprintw(18,0,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[10]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(20,0,"X");
        mvprintw(21,0,"X");
        mvprintw(22,0,"X");
        mvprintw(23,0,"X");
        mvprintw(24,0,"X");
        mvprintw(25,0,"X");
        mvprintw(26,0,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[12]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(28,0,"X");
        mvprintw(29,0,"X");
        mvprintw(30,0,"X");
        mvprintw(31,0,"X");
        mvprintw(32,0,"X");
        mvprintw(33,0,"X");
        mvprintw(34,0,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[14]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(36,0,"X");
        mvprintw(37,0,"X");
        mvprintw(38,0,"X");
        mvprintw(39,0,"X");
        mvprintw(40,0,"X");
        mvprintw(41,0,"X");
        mvprintw(42,0,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[9]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(12,46,"X");
        mvprintw(13,46,"X");
        mvprintw(14,46,"X");
        mvprintw(15,46,"X");
        mvprintw(16,46,"X");
        mvprintw(17,46,"X");
        mvprintw(18,46,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[11]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(20,46,"X");
        mvprintw(21,46,"X");
        mvprintw(22,46,"X");
        mvprintw(23,46,"X");
        mvprintw(24,46,"X");
        mvprintw(25,46,"X");
        mvprintw(26,46,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[13]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(28,46,"X");
        mvprintw(29,46,"X");
        mvprintw(30,46,"X");
        mvprintw(31,46,"X");
        mvprintw(32,46,"X");
        mvprintw(33,46,"X");
        mvprintw(34,46,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[15]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(36,46,"X");
        mvprintw(37,46,"X");
        mvprintw(38,46,"X");
        mvprintw(39,46,"X");
        mvprintw(40,46,"X");
        mvprintw(41,46,"X");
        mvprintw(42,46,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[6]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(12,47,"X");
        mvprintw(13,47,"X");
        mvprintw(14,47,"X");
        mvprintw(15,47,"X");
        mvprintw(16,47,"X");
        mvprintw(17,47,"X");
        mvprintw(18,47,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[4]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(20,47,"X");
        mvprintw(21,47,"X");
        mvprintw(22,47,"X");
        mvprintw(23,47,"X");
        mvprintw(24,47,"X");
        mvprintw(25,47,"X");
        mvprintw(26,47,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[2]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(28,47,"X");
        mvprintw(29,47,"X");
        mvprintw(30,47,"X");
        mvprintw(31,47,"X");
        mvprintw(32,47,"X");
        mvprintw(33,47,"X");
        mvprintw(34,47,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[0]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(36,47,"X");
        mvprintw(37,47,"X");
        mvprintw(38,47,"X");
        mvprintw(39,47,"X");
        mvprintw(40,47,"X");
        mvprintw(41,47,"X");
        mvprintw(42,47,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[7]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(12,93,"X");
        mvprintw(13,93,"X");
        mvprintw(14,93,"X");
        mvprintw(15,93,"X");
        mvprintw(16,93,"X");
        mvprintw(17,93,"X");
        mvprintw(18,93,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[5]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(20,93,"X");
        mvprintw(21,93,"X");
        mvprintw(22,93,"X");
        mvprintw(23,93,"X");
        mvprintw(24,93,"X");
        mvprintw(25,93,"X");
        mvprintw(26,93,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[3]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(28,93,"X");
        mvprintw(29,93,"X");
        mvprintw(30,93,"X");
        mvprintw(31,93,"X");
        mvprintw(32,93,"X");
        mvprintw(33,93,"X");
        mvprintw(34,93,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
      if(place[1]==1){
      attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
        mvprintw(36,93,"X");
        mvprintw(37,93,"X");
        mvprintw(38,93,"X");
        mvprintw(39,93,"X");
        mvprintw(40,93,"X");
        mvprintw(41,93,"X");
        mvprintw(42,93,"X");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(1));
      }
}

void affichageMessage(){


mvprintw(10, 100,"                                                             _       _                   _");
mvprintw(11, 100,"   __ _ _ __  _ __  _   _ _   _  ___ _ __   ___ _   _ _ __  | | __ _| |_ ___  _   _  ___| |__   ___");
mvprintw(12, 100,"  / _` | '_ \\| '_ \\| | | | | | |/ _ \\ '__| / __| | | | '__| | |/ _` | __/ _ \\| | | |/ __| '_ \\ / _ \\ ");
mvprintw(13, 100," | (_| | |_) | |_) | |_| | |_| |  __/ |    \\__ \\ |_| | |    | | (_| | || (_) | |_| | (__| | | |  __/ ");
mvprintw(14, 100,"  \\__,_| .__/| .__/ \\__,_|\\__, |\\___|_|    |___/\\__,_|_|    |_|\\__,_|\\__\\___/ \\__,_|\\___|_| |_|\\___|");
mvprintw(15, 100,"       |_|   |_|  _       |___/                                 _ _   _");
mvprintw(16, 100,"                 ( ) __ _( )  _ __   ___  _   _ _ __ __ _ _   _(_) |_| |_ ___ _ __");
mvprintw(17, 100,"                 |/ / _` |/  | '_ \\ / _ \\| | | | '__/ _` | | | | | __| __/ _ \\ '__|");
mvprintw(18, 100,"                   | (_| |   | |_) | (_) | |_| | | | (_| | |_| | | |_| ||  __/ |");
mvprintw(19, 100,"                    \\__,_|   | .__/ \\___/ \\__,_|_|  \\__, |\\__,_|_|\\__|\\__\\___|_|");
mvprintw(20, 100,"                             |_|                       |_|");


}

void automatique(){

	clear();
	refresh();
	srand(time(NULL));
  initColor();

	int nbLin, nbCol;
	char ** mat;
	char touche = 'e';

  mat=init_tab2(&nbLin, &nbCol);

  loadModels();

int montant=0;
int montant2=0;
int montant3=0;

  int place[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	VEHICULE * voiture1 = NULL;

	voiture1 = malloc(sizeof(VEHICULE));
		voiture1->posx = 68;
		voiture1->posy = 50;
		voiture1->direction = 'N';
		voiture1->type = '1';
    voiture1->etat='D';
    voiture1->tps = 0;
    voiture1->duree=rand()%100+30;
    voiture1->couleur=rand()%6+1;
		//voitureAct = voiture1;
		voiture1->NXT = NULL;
		VEHICULE * voitureAct=voiture1;
		//ajoutVoit(voiture1);
		int nombVoit=1;

		while(1){
			erase();
			affichematrice(nbLin,nbCol, mat);
      affPlace();
      affichageMessage();
      montant2=montant%100;
      montant3=(montant-montant2)/100;
      mvprintw(55,0,"Le parking vous rapporte pour l'instant la somme de : %d$%d", montant3, montant2);

			voitureAct = voiture1;
			do{
				if(voitureAct->posy<51){
		    	deplacementVoiture2(mat,voitureAct, place, voiture1);
          attroff(COLOR_PAIR(1));
          attron(COLOR_PAIR(voitureAct->couleur));
					printVehicule(voitureAct);
          attroff(COLOR_PAIR(voitureAct->couleur));
          attron(COLOR_PAIR(1));
          //mvprintw(20,5, "%c %c %d", mat[voitureAct->posy][voitureAct->posx], voitureAct->etat, nombVoit);
          //refresh();
				}
        if(voitureAct->posy==50){
          if(voitureAct->etat=='N'){
            montant+=voitureAct->duree;
          }
					ajoutVoit(voitureAct);
          //nombVoit++;
				}
				if(voitureAct->posy==40 && voitureAct->posx==68 && nombVoit<18){
					ajoutVoit(voitureAct);
					nombVoit++;
				}

				voitureAct=voitureAct->NXT;
			}while(voitureAct!=NULL);
      affichePlace(place);
			refresh();
			usleep(60000);

			touche=key_pressed();
			if(touche=='a'){
				break;
			}
		}
		erase();
		affichematrice(nbLin,nbCol, mat);
    affPlace();
    affichePlace(place);
    voitureAct = voiture1;
    do{
      if(voitureAct->posy<51){
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(voitureAct->couleur));
    printVehicule(voitureAct);
    attroff(COLOR_PAIR(voitureAct->couleur));
    attron(COLOR_PAIR(1));
  }
  voitureAct=voitureAct->NXT;
    }while(voitureAct!=NULL);
    clear();
    mvprintw(55,0,"Le parking vous a rapporté la somme de : %d$%d", montant3, montant2);
    move(0,0);
        char c;
        FILE * fic = fopen("fin.txt", "r");
        while ((c=fgetc(fic)) != EOF){
                printw("%c", c);
        }
        fclose(fic);

        printw("\n\n");

        FILE * ficc = fopen("espace.txt", "r");
        while ((c=fgetc(ficc)) != EOF){
                printw("%c", c);
        }
        fclose(ficc);
	
    refresh();
    touche ='r';
    while(touche!=' '){
      touche=key_pressed();
    }
	//sleep(2);
}
