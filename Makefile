SOURCE = ballAlg.c	
TARGET = ballAlg
CC = gcc
FLAGS = -fopenmp -O3 -lm
#FLAGS = -Wall -Wextra -Wpedantic -Wformat

all: ballAlg

ballAlg: ballAlg.c
	$(CC) $(SOURCE) -o $(TARGET) $(FLAGS)

clean: 
	@echo 
	@echo ----  Cleaning project ...  ----
	@echo 
	rm -f $(TARGET) ./tests/*.my.out ./tests/*.diff
	@echo
	@echo ----  Very nice!  ----
