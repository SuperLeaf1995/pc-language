#Potato-C Makefile for GCC/DJGPP
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -ansi -I$(SRC_DIR)

#Directories
SRC_DIR = src
OUT_DIR = bin

#Installation variables
INSTALL_PATH = /usr/local/bin/potato-c
APP_NAME = potato-c.o

all: $(OUT_DIR)/pc.o
	echo Done

$(OUT_DIR)/pc.o: $(SRC_DIR)/main.c $(OUT_DIR)/parser.obj $(OUT_DIR)/macro.obj $(OUT_DIR)/strfunc.obj $(OUT_DIR)/warning.obj
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/parser.obj: $(SRC_DIR)/parser.c $(OUT_DIR)/strfunc.obj
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OUT_DIR)/macro.obj: $(SRC_DIR)/macro.c $(OUT_DIR)/strfunc.obj $(OUT_DIR)/warning.obj
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OUT_DIR)/strfunc.obj: $(SRC_DIR)/strfunc.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(OUT_DIR)/warning.obj: $(SRC_DIR)/warning.c
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
