#include "macro.h"
#include "strfunc.h"
#include "warning.h"

void define(char * name, size_t value) {
	fprintf(stdout,"DEFINED MACRO %s WITH VALUE %u\n",name,value);
	return;
}

int parseMacros(register char * str) {
	size_t i; char * locStr; /*the local string and etc*/
	char * macroName; size_t offForDelete; /*delete macro after definition*/
	size_t offOfLocStr; /*to separate arguments from macro*/
	int newlineCounter = 0;
	
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"\n",strlen("\n")) == 0) {
			newlineCounter++;
		}
		
		if(strncmp(str+i,"#",strlen("#")) == 0) { /*macro preprocessor statement!*/
			offForDelete = i;
			
			i++; /*skip the hashtag*/
			
			locStr = getName(str+i); /*allocate memory for local string*/
			
			if(locStr == NULL) {
				return 3;
			} /*return*/
			
			i += strlen(locStr);
			
			if(strncmp(locStr,"type",strlen("type")) == 0) { /*a macro is defined!*/
				offOfLocStr = i;
				offOfLocStr += skipWhileMatch(str+offOfLocStr," ");
				
				if(locStr != NULL) {
					free(locStr);
				} /*get DEFINE arguments*/
				
				if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) { /*type has no name*/
					warning(WARNING_UNNAMED_DEFINE,newlineCounter);
				} else { /*type has name*/
					/*Get argument 1: Define name*/
					locStr = getName(str+offOfLocStr); /*allocate memory for local string*/
					if(locStr == NULL) {
						return 2;
					} /*return if no memory available*/
					
					i += strlen(locStr); /*add local string strlen to total iteration*/
					offOfLocStr += strlen(locStr); /*offse of locstr (sum the string lenght of locstr)*/
					
					fprintf(stdout,"Define %s\n",locStr);
					
					macroName = malloc(strlen(locStr)+1); /*free LOCSTR for define value, but*/
					if(macroName == NULL) {
						return 4;
					} /*copy the contents of the macro name*/
					
					memcpy(macroName,locStr,strlen(locStr)+1); /*to a new place*/
					
					if(locStr != NULL) {
						free(locStr);
					} /*free locstr*/
				
					/*Implicit define macro*/
					if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) { /*Check if arguments exists*/
						warning(WARNING_IMPLICIT_DEFINE,newlineCounter); /*warn for implicit define*/
						
						fprintf(stdout,"(Implicit) Value %d\n",0);
						
						replaceWith(str,macroName,"0"); /*replace macro with default implicit*/
						
						removeBetween(str+offForDelete,"#","\n");
					} else { /*Explicit define macro*/
						offOfLocStr += skipWhileMatch(str+offOfLocStr," "); /*skip all spaces*/
						
						locStr = getName(str+offOfLocStr); /*get argument 2*/
						if(locStr == NULL) {
							return 4;
						}
						
						fprintf(stdout,"(Explicit) Value %s\n",locStr);
						
						replaceWith(str,macroName,locStr); /*replace instances of the macro with the value*/
						
						i += strlen(locStr);
						
						removeBetween(str+offForDelete,"#","\n");
						
						if(locStr != NULL) {
							free(locStr);
						} /*free memory to avoid memory leaks*/
					}
					if(macroName != NULL) {
						free(macroName);
					}
				}
			} else {
				if(locStr != NULL) {
					free(locStr);
				}
			}
		}
	}
	return 0; /*end*/
}
