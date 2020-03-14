#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

typedef struct function {
	char * name;
	char * params;
	unsigned char n_params;
}function;

#include "function.c"

#endif
