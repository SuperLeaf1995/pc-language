#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int removeBetween(char * str, const char* open, const char* close);
int removeAll(char * str, const char* rem);
void parsetDebug(FILE * _s, char * str);

#endif
