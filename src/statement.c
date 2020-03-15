/* statement.c
 * Create,Delete,Add translations and more stuff to modify or fit
 * statements.
 * 
 * In Potato-C statements are small pieces of code wich are
 * abstract representations of the actual instruction to be
 * transpiled*/

void createStatement(statement * s, const char* a, unsigned char n) {
	if(a == NULL) {
		fprintf(stderr,"Null string on createStatement\n");
		abort();
	}
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

void addStatementTranslation(statement * s, const char* a, unsigned char p) {
	if(a == NULL) {
		fprintf(stderr,"Null string on addStatementTranslation\n");
		abort();
	}
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
	if(!t) {
		fprintf(stderr,"Invalid stream given on statementInfo\n");
		abort();
	}
	fprintf(t,"Name: %s\n",s->name);
	fprintf(t,"No. of parameters: %u\n",s->n_params);
	fprintf(t,"Translations\n");
	for(i = 0; i < 64; i++) {
		if(s->translation[i] != NULL) {
			fprintf(t,"%u : %s\n",i,s->translation[i]);
		}
	}
	return;
}

/* statementToAssembly(
 * f = Stream to output assembly code
 * s = Target statement
 * p = Target platform
 * t = Statement parameters, separated by a comma (NULL = No parameters)
 * )*/
void statementToAssembly(FILE * f, statement * s, unsigned char p, char * t) {
	size_t i = 0;
	int i2 = 0;
	int i3 = 0;
	char * tx;
	if(t == NULL) {
		fprintf(stderr,"No parameter statement\n");
		return;
	}
	if(s->translation[p] == NULL) {
		if(s->name != NULL) {
			fprintf(stderr,"Illegal access to out-of-bounds translation entry %u for statement (%s)\n",p,s->name);
		} else {
			fprintf(stderr,"Illegal access to out-of-bounds for void statement\n");
		}
		return;
	}
	for(i = 0; i < strlen(s->translation[p]); i++) {
		if(s->translation[p][i] == '$') { /*parameter*/
			i++; i3 = 0;
			i2 = atoi(s->translation[p]+i);
			tx = strtok(t,",");
			while(tx != NULL) {
				if(i2 == i3) {
					fprintf(f,"%s",tx);
					break;
				}
				tx = strtok(NULL,",");
				i3++;
			}
		} else if(s->translation[p][i] == '*') { /*string*/
			i2 = 0;
			while(t[i2] != '"') {
				i2++;
			} i2++;
			while(t[i2] != '"') {
				fputc(t[i2],f);
				i2++;
			}
		} else {
			fputc(s->translation[p][i],f);
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
