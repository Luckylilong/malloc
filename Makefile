all: malloc main churn malloc_simple

malloc: malloc.c malloc.h
	gcc -g -c malloc.c

malloc_simple: malloc_simple.c
	gcc --std=c99 -g -Wall -o simple malloc_simple.c malloc.o

main: main.c malloc.o
	gcc --std=c99 -g -Wall -o bin main.c malloc.o

churn: churn.c malloc.o
	gcc --std=c99 -g -Wall -o churn churn.c malloc.o
