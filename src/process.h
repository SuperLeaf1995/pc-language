#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#if defined(__GNUC__)
#include <memory.h>
#endif

#include "token.h"

void preProcess(int s, char * fd);

#include "process.c"

#endif
