/* token.c
 * Create/Delete and track tokens*/

void createToken(token * t, char * n, signed char match, signed char mgroup) {
	if(t->str == NULL) {
		t->str = malloc(strlen(n));
		if(t->str == NULL) {
			fprintf(stderr,"Token allocation error\n");
			abort();
		}
	}
	t->match = match;
	t->mgroup = mgroup;
	return;
}

void deleteToken(token * t) {
	if(t->str != NULL) {
		free(t->str);
	}
	t->match = 0;
	t->mgroup = 0;
	return;
}
