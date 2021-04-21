SEQSOURCE = ballAlg.c	
SEQTARGET = ballAlg
PARSOURCE = ballAlg-omp.c	
PARTARGET = ballAlg-omp
CC = gcc
FLAGS = -fopenmp -O3 -lm

all: ballAlg

ballAlg: ballAlg.c ballAlg-omp.c
	$(CC) $(SEQSOURCE) -o $(SEQTARGET) $(FLAGS)
	$(CC) $(PARSOURCE) -o $(PARTARGET) $(FLAGS)
