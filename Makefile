all: malloc main churn

malloc: malloc.c malloc.h
	gcc -g -c malloc.c

main: main.c malloc.o
	gcc --std=c99 -g -Wall -lm -o bin main.c malloc.o

churn: churn.c malloc.o
	gcc --std=c99 -g -Wall -lm -o churn churn.c malloc.o
