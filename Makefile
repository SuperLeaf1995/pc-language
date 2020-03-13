CFLAGS = -Wall -std=c89
CC = gcc

test:
	$(CC) $(CFLAGS) src/main.c -o out/potato.o
