#include "strfunc.h"

int removeBetween(register char * str, register const char* open, register const char* close) {
	size_t i; size_t openPos; size_t closePos;
	openPos = 0; closePos = 0;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,open,strlen(open)) == 0) {
			openPos = i;
		} else if(strncmp(str+i,close,strlen(close)) == 0) {
			closePos = i+strlen(close);
			memmove(str+openPos,str+closePos,strlen(str)-i);
		}
	}
	return 1;
}

int removeAll(register char * str, register const char* rem) {
	size_t i;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,rem,strlen(rem)) == 0) {
			memmove(str+i,str+i+strlen(rem),(strlen(str)-i));
			i--;
		}
	}
	return 1;
}

char * getName(register char * str) {
	size_t name_len = 0; char * str_ret;
	while(strncmp(str+name_len," ",strlen(" ")) != 0
	&& strncmp(str+name_len,"\n",strlen("\n")) != 0
	&& strncmp(str+name_len,";",strlen(";")) != 0) {
		name_len++;
	}
	str_ret = malloc(name_len+1);
	if(str_ret == NULL) { return NULL; }
	strncpy(str_ret,str,name_len); str_ret[name_len] = 0;
	return str_ret;
}

char * getUntilMatch(register char * str, register const char* m) {
	size_t name_len = 0;
	char * str_ret;
	while(strncmp(str+name_len,m,strlen(m)) != 0) {
		name_len++;
	}
	str_ret = malloc(name_len+1);
	if(str_ret == NULL) { return NULL; }
	strncpy(str_ret,str,name_len); str_ret[name_len] = 0;
	return str_ret;
}

size_t skipUntilMatch(register char * str, register const char* m) {
	size_t i = 0;
	while(strncmp(str+i,m,strlen(m)) != 0) {
		i++;
		if(i > strlen(str)) { return 0; }
	}
	return i;
}

size_t skipWhileMatch(register char * str, register const char* m) {
	size_t i = 0;
	while(strncmp(str+i,m,strlen(m)) == 0) {
		i++;
		if(i > strlen(str)) { return strlen(str); }
	}
	return i;
}

size_t hasAsciiBefore(register char * str, register const char* b) {
	size_t i = 0;
	while(strncmp(str+i,b,strlen(b)) != 0) {
		if(isascii(str[i])) { return 1; }
		i++;
		if(i > strlen(str)) { break; }
	}
	return 0;
}
