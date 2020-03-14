#ifndef STATEMENT_H
#define STATEMENT_H

typedef struct _statement {
	char * name;
	unsigned char n_params;
	char * translation[64];
	unsigned char n_translation;
}statement;

#endif
