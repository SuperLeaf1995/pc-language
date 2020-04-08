#include "macro.h"
#include "strfunc.h"
#include "warning.h"

static int cont = 0; /*Used to count macro definitions (i.e, define macro 1 in 1, 2 in 2, etc)*/

const char * strzero = "0";

void define(struct macro * m) {
	fprintf(stdout,"DEFINED MACRO %s WITH VALUE %s\n",m[cont].name,m[cont].literalValue);
	cont++;
	return;
}

int parseMacros(struct macro * m, register char * str) {
	size_t i; char * locStr; /*the local string and etc*/
	char * macroName; size_t offForDelete; /*delete macro after definition*/
	size_t offOfLocStr; /*to separate arguments from macro*/
	int newlineCounter = 0;
	
	/*Allocate space for macros*/
	m = malloc(32*sizeof(struct macro));
	if(m == NULL) { return 13; }
	
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"\n",strlen("\n")) == 0) { newlineCounter++; }
		
		/*macro preprocessor statement is coming*/
		if(strncmp(str+i,"#",strlen("#")) == 0) {
			offForDelete = i; /*delete everything (deleteOffset)*/
			i++; /*skip the first char*/
			
			/*Get the macro statement (see below)*/
			locStr = getName(str+i);
			if(locStr == NULL) { return 3; } /*If there is no macro statement, bail out*/
			i += strlen(locStr); /*Pad out and go where the arguments of the macro stat. are*/
			
			/*A new macro is defined*/
			if(strncmp(locStr,"define",strlen("define")) == 0) {
				offOfLocStr = i;
				offOfLocStr += skipWhileMatch(str+offOfLocStr," ");
				
				if(locStr != NULL) {
					free(locStr);
				} /*get DEFINE arguments*/
				
				if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) { /*type has no name*/
					warning(WARNING_UNNAMED_DEFINE,newlineCounter);
				} else { /*Define has name?*/
					/*Get argument 1: Define name*/
					
					/*Allocate memory for local string*/
					locStr = getName(str+offOfLocStr);
					if(locStr == NULL) { return 2; }
					
					/*Add local string strlen to total iteration*/
					/*offset of locstr (sum the string lenght of locstr)*/
					i += strlen(locStr);
					offOfLocStr += strlen(locStr);
					
					/*Save the name in the macro struct*/
					m[cont].name = malloc(strlen(locStr)+1);
					if(m[cont].name == NULL) { return 10; }
					strcpy(m[cont].name,locStr);
					
					/*Free LOCSTR for define value, but
					copy the contents of the macro name
					to a new place*/
					macroName = malloc(strlen(locStr)+1);
					if(macroName == NULL) { return 4; }
					memcpy(macroName,locStr,strlen(locStr)+1);
					
					/*Free locstr*/
					if(locStr != NULL) { free(locStr); }
				
					/*Implicit define macro*/
					/*Check for define's value*/
					if(hasAsciiBefore(str+offOfLocStr,"\n") == 0) {
						warning(WARNING_IMPLICIT_DEFINE,newlineCounter);

						/*Define the new macro*/
						m[cont].literalValue = malloc(strlen(locStr)+1);
						if(m[cont].literalValue == NULL) { return 11; }
						strcpy(m[cont].literalValue,strzero);
						define(m);
						
						removeBetween(str+offForDelete,"#","\n");
					} else { /*Explicit define macro*/
						offOfLocStr += skipWhileMatch(str+offOfLocStr," "); /*skip all spaces*/
						
						locStr = getName(str+offOfLocStr); /*get argument 2*/
						if(locStr == NULL) {
							return 4;
						}

						m[cont].literalValue = malloc(strlen(locStr)+1);
						if(m[cont].literalValue == NULL) {
							return 12;
						}
						strcpy(m[cont].literalValue,locStr);
						define(m);
						
						/*Skip value lenght (we should be in another line right now)*/
						i += strlen(locStr);
						if(locStr != NULL) { free(locStr); }
					}
					if(macroName != NULL) { free(macroName); }
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
