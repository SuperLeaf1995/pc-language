#ifndef STRFUNC_H
#define STRFUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifdef __USE_CTYPE__ /*Compatibility with old compilers*/
#include <ctype.h>
#else
#define isascii(c) (((unsigned char)(c)) <= 0x7f)
#endif

int removeBetween(register char * str, register const char* open, register const char* close);
int removeAll(register char * str, register const char* rem);
char * getName(register char * str);
char * getUntilMatch(register char * str, register const char* m);
size_t skipUntilMatch(register char * str, register const char* m);
size_t skipWhileMatch(register char * str, register const char* m);
size_t hasAsciiBefore(register char * str, register const char* m);

#endif
