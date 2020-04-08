#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct macro {
	char * name; /*Name of the macro*/
	char * literalValue; /*Value of the macro*/
};

void define(struct macro * m);
int parseMacros(struct macro * m, register char * str);

#if defined(__cplusplus)
}
#endif

#endif
