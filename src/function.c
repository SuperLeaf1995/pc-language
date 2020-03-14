#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "function.h"

void createFunction(function * f, char * n, char * p) {
	f->name = malloc(strlen(n)+1);
	if(f->name == NULL) {
		fprintf(stderr,"Cannot create function name\n");
		abort();
	}
	f->params = malloc(strlen(p)+1);
	if(f->params == NULL) {
		fprintf(stderr,"Cannot create function parameters\n");
		abort();
	}
	strcpy(f->name,n);
	strcpy(f->params,p);
	return;
}

void deleteFunction(function * f) {
	if(f->name != NULL) { free(f->name); }
	if(f->params != NULL) { free(f->params); }
	return;
}
