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

int parseMacros(register char * str);

#endif
