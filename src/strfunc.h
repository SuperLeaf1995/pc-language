#ifndef STRFUNC_H
#define STRFUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int removeBetween(register char * str, register const char* open, register const char* close);
int removeAll(register char * str, register const char* rem);
char * getName(register char * str);
char * getUntilMatch(register char * str, register const char* m);
size_t skipUntilMatch(register char * str, register const char* m);
size_t skipWhileMatch(register char * str, register const char* m);

#endif
