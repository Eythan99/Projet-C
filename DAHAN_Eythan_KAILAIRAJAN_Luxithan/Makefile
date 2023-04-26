all: header.o jeu.o compilation execution

header.o: header.c header.h
	gcc -Wall -c header.c -o header.o -lncurses

jeu.o: jeu.c
	gcc -Wall -c jeu.c -o jeu.o -lncurses

compilation:
	gcc -Wall jeu.o header.o -o jeu -lncurses

execution:
	./jeu
