#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "statement.h"

void createStatement(statement * s, char * a, unsigned char n) {
	s->name = malloc(strlen(a)+1);
	if(s->name == NULL) {
		fprintf(stderr,"Cannot create statement\n");
		abort();
	}
	s->n_params = n;
	return;
}

void addStatementTranslation(statement * s, char * a, unsigned char p) {
	s->translation[p] = malloc(strlen(a)+1);
	if(s->translation[p] == NULL) {
		fprintf(stderr,"Cannot add statement translation\n");
		abort();
	}
	return;
}

void statementToAssembly(FILE * f, statement * s, unsigned char p) {
	if(s->translation[p] == NULL) {
		fprintf(stderr,"Illegal access to out-of bounds translation entry\n");
		abort();
	}
	fprintf(f,"%s",s->translation[p]);
	return;
}
