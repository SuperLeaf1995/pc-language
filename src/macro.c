#include "macro.h"

char * getName(char * str) {
	size_t name_len = 0; char * str_ret;
	while(strncmp(str+name_len," ",strlen(" ")) != 0
	&& strncmp(str+name_len,"\n",strlen("\n")) != 0
	&& strncmp(str+name_len,";",strlen(";")) != 0) {
		name_len++;
	}
	str_ret = malloc(name_len+1);
	if(str_ret == NULL) { return NULL; }
	strncpy(str_ret,str,name_len);
	return str_ret;
}

char * getUntilMatch(char * str, const char* m) {
	size_t name_len = 0;
	char * str_ret;
	while(strncmp(str+name_len,m,strlen(m)) != 0) {
		name_len++;
	}
	str_ret = malloc(name_len+1);
	if(str_ret == NULL) { return NULL; }
	strncpy(str_ret,str,name_len);
	return str_ret;
}

size_t skipUntilMatch(char * str, const char* m) {
	size_t i = 0;
	while(strncmp(str+i,m,strlen(m)) != 0) {
		i++;
		if(i > strlen(str)) { return 0; }
	}
	return i;
}

size_t skipWhileMatch(char * str, const char* m) {
	size_t i = 0;
	while(strncmp(str+i,m,strlen(m)) == 0) {
		i++;
		if(i > strlen(str)) { return 0; }
	}
	return i;
}

struct macro * parseMacros(char * str) {
	struct macro * macro_r; size_t i; char * locStr; size_t macrosToAllocate = 0;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"#",strlen("#")) == 0) {
			i++; macrosToAllocate++;
			locStr = getName(str+i);
			if(locStr == NULL) { return NULL; }
			i += strlen(locStr);
			free(locStr); /*free memory to avoid memory leaks*/
		}
	}
	macro_r = malloc(macrosToAllocate*sizeof(struct macro));
	if(macro_r == NULL) { return NULL; }
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"#",strlen("#")) == 0) {
			i++; locStr = getName(str+i); /*get macro name (define,ifdef,etc)*/
			if(locStr == NULL) { return NULL; }
			i += strlen(locStr);
			fprintf(stdout,"MACRO DIRECTIVE %s\n",locStr);
			
			i += skipWhileMatch(str+i," "); /*skip until finding arguments*/
			
			locStr = getUntilMatch(str+i,"\n"); /*get arguments for macro*/
			if(locStr == NULL) { return NULL; }
			i += strlen(locStr);
			fprintf(stdout,"MACRO ARGUMENTS %s\n",locStr);
			free(locStr);
		}
	}
	return macro_r;
}
