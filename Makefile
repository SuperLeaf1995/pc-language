CFLAGS = -Wall -std=c89
CC = gcc

test:
	$(CC) $(CFLAGS) potato.c -o potato.o
