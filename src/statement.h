#ifndef STATEMENT_H
#define STATEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct statement {
	char * name;
	unsigned char n_params;
	char * translation[64];
	unsigned char n_translation;
}statement;

#include "statement.c"

#endif
