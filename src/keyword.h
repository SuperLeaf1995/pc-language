#ifndef KEYWORD_H
#define KEYWORD_H

#include <stdio.h>
#include <stdlib.h>
#if defined(__GNUC__)
#include <memory.h>
#endif
#include <string.h>

typedef keyword {
	char * name;
}keyword;

#endif
