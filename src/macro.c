#include "macro.h"
#include "strfunc.h"
#include "warning.h"

void define(char * name, size_t value) {
	fprintf(stdout,"DEFINED MACRO %s WITH VALUE %u\n",name,value);
	return;
}

int parseMacros(register char * str) {
	size_t i; char * locStr; /*the local string and etc*/
	size_t offOfLocStr; /*to separate arguments from macro*/
	int newlineCounter = 0;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"\n",strlen("\n")) == 0) {
			newlineCounter++;
		}
		if(strncmp(str+i,"#",strlen("#")) == 0) {
			i++;
			locStr = getName(str+i); /*allocate memory for local string*/
			if(locStr == NULL) { return 3; } /*return*/
			i += strlen(locStr);
			
			if(strncmp(locStr,"define",strlen("define")) == 0) {
				offOfLocStr = i;
				offOfLocStr += skipWhileMatch(str+offOfLocStr," ");
				if(locStr != NULL) { free(locStr); } /*get DEFINE arguments*/
				if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) { /*Check if arguments exists*/
					warning(WARNING_UNNAMED_DEFINE,newlineCounter);
				} else {
					/*Get argument 1: Define name*/
					locStr = getName(str+offOfLocStr); /*allocate memory for local string*/
					if(locStr == NULL) { return 2; } /*return*/
					i += strlen(locStr); /*add local string strlen to total iteration*/
					offOfLocStr += strlen(locStr);
					fprintf(stdout,"Define %s\n",locStr);
					if(locStr != NULL) { free(locStr); }
				
					if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) { /*Check if arguments exists*/
						warning(WARNING_IMPLICIT_DEFINE,newlineCounter);
					} else {
						/*Argument 2: Define value*/
						offOfLocStr += skipWhileMatch(str+offOfLocStr," "); /*skip all spaces*/
						locStr = getName(str+offOfLocStr);
						if(locStr == NULL) { return 4; }
						fprintf(stdout,"Value %s\n",locStr);
						i += strlen(locStr);
						if(locStr != NULL) { free(locStr); } /*free memory to avoid memory leaks*/
					}
				}
			} else {
				if(locStr != NULL) { free(locStr); }
			}
		}
	}
	return 0; /*end*/
}
