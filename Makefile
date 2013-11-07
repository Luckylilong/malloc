all: malloc main

malloc: malloc.c malloc.h
	gcc -c malloc.c

main: main.c malloc.o
	gcc --std=c99 -g -Wall -o bin main.c malloc.o
