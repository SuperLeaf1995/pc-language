#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

struct macro {
	char * name; /*Name of the macro*/
	char * literalValue; /*Value of the macro*/
};

char * getName(char * str);
char * getUntilMatch(char * str, const char* m);
size_t skipUntilMatch(char * str, const char* m);
size_t skipWhileMatch(char * str, const char* m);
struct macro * parseMacros(char * str);

#endif
