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

void createStatement(statement * s, const char* a, unsigned char n);
void addStatementTranslation(statement * s, const char* a, unsigned char p);
void statementInfo(FILE * t, statement * s);
void statementToAssembly(FILE * f, statement * s, unsigned char p, char* t);
void deleteStatement(statement * s);

#include "statement.c"

#endif
