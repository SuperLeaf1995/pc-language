#ifndef WARNING_H
#define WARNING_H

#include <stdio.h>
#include <stdlib.h>

#define TOTAL_WARNINGS 2
#define WARNING_IMPLICIT_DEFINE 1
#define WARNING_UNNAMED_DEFINE 2

void warning(int warn, int line);

#endif
