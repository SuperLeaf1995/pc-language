#Potato-C Makefile for GCC/DJGPP
CC = gcc
CFLAGS = -Wall -Werror -Wstrict-prototypes -ansi -pedantic -Wextra -I$(SRC_DIR) -std=c89

#Directories
SRC_DIR = src
OUT_DIR = bin

#Installation variables
INSTALL_PATH = /usr/local/bin/potato-c
APP_NAME = potato-c.o

all: $(OUT_DIR)/potato-c.o
	echo Done

$(OUT_DIR)/potato-c.o: $(SRC_DIR)/main.c $(OUT_DIR)/parser.obj $(OUT_DIR)/macro.obj
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/parser.obj: $(SRC_DIR)/parser.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OUT_DIR)/macro.obj: $(SRC_DIR)/macro.c
	$(CC) $(CFLAGS) -c $^ -o $@

#Install Potato-C
install: $(OUT_DIR)/potato-c.o
	mkdir $(INSTALL_PATH)
	cp $(OUT_DIR)/potato-c.o $(INSTALL_PATH)/potato-c.o

#Removing Potato-C from system
unistall:
	rm -rf $(INSTALL_PATH)

#Clean
clean:
	rm -f $(OUT_DIR)/*.obj
