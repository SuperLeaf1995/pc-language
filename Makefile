CFLAGS = -Wall -Wextra -std=c89 -O3 -g
CC = gcc

test:
	$(CC) $(CFLAGS) src/main.c -o bin/pc.o
