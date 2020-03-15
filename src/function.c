/* function.c
 * Creates/Deletes and modifies a function
 * in Potato-C a function is made out of a
 * routine wich can have different parameters
*/

void createFunction(function * f, const char* n, const char* p) {
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
