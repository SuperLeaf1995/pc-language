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
	strcpy(s->name,a);
	s->n_params = n;
	s->n_translation = 0;
	return;
}

void addStatementTranslation(statement * s, char * a, unsigned char p) {
	s->translation[p] = malloc(strlen(a)+1);
	if(s->translation[p] == NULL) {
		fprintf(stderr,"Cannot add statement translation\n");
		abort();
	}
	strcpy(s->translation[p],a);
	s->n_translation++;
	return;
}

void statementInfo(FILE * t, statement * s) {
	unsigned char i;
	fprintf(t,"Name: %s\n",s->name);
	fprintf(t,"No. of parameters: %u\n",s->n_params);
	fprintf(t,"Translations\n");
	for(i = 0; i < 64; i++) {
		if(s->translation[i] != NULL) {
			fprintf(t,"%s\n",s->translation[i]);
		}
	}
	return;
}

void statementToAssembly(FILE * f, statement * s, unsigned char p, char * t) {
	unsigned long long i;
	unsigned long long i2;
	unsigned long long i3;
	char * tx;
	if(s->translation[p] == NULL) {
		fprintf(stderr,"Illegal access to out-of bounds translation entry %u for statement (%s)\n",p,s->name);
		abort();
	}
	for(i = 0; i < strlen(t); i++) {
		if(t[i] == '$') {
			i++;
			i2 = atol(t+i);
			tx = strtok(t,",");
			while(tx != NULL) {
				if(i2 == i3) {
					fprintf(f,"%s",tx);
					break;
				}
				tx = strtok(NULL,",");
				i3++;
			}
		} else {
			fputc(t[i],f);
		}
	}
	return;
}

void deleteStatement(statement * s) {
	unsigned char i;
	for(i = 0; i < s->n_translation; i++) {
		if(s->translation[i] != NULL) {
			free(s->translation[i]);
		}
	}
	s->n_translation = 0;
	s->n_params = 0;
	free(s->name);
	return;
}
