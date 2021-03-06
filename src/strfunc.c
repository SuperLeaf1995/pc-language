#include "strfunc.h"

int removeBetween(register char * str, register const char* open, register const char* close) {
	size_t i; size_t openPos; size_t closePos; char opened = 0;
	openPos = 0; closePos = 0;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,open,strlen(open)) == 0) {
			openPos = i; opened = 1;
		} else if(strncmp(str+i,close,strlen(close)) == 0 && opened == 1) {
			closePos = i+strlen(close);
			memmove(str+openPos,str+closePos,strlen(str)-i);
			opened = 0;
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

size_t hasAsciiBefore(register char * str, register const char* m) {
	size_t i = 0;
	while(strncmp(str+i,m,strlen(m)) != 0) {
		if(isascii(str[i])) { return 1; }
		i++;
		if(i > strlen(str)) { break; }
	}
	return 0;
}

/*replaces M with B*/
void replaceWith(register char * str, register const char * m, register const char * b) {
	size_t i;
	char * tmpbuf;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,m,strlen(m)) == 0) {
			if(strlen(m) == strlen(b)) { /*b and m are equal in lenght*/
				printf("EQUAL\n");
				memcpy(str+i,b,strlen(b));
			} else if(strlen(m) > strlen(b)) { /*m is more lenghty than b*/
				/*Remove M (but leave space for B) and then place B*/
				memmove(str+i,str+i+(strlen(m)-strlen(b)),strlen(str)-i);
				memcpy(str+i,b,strlen(b));
			} else {
				/*B is bigger than M, move the string and leave space for B*/
				/*moving the string forward is utterly hard*/
				/*allocate some memory for a tempbuf*/
				tmpbuf = malloc((strlen(str)-i)+1);
				if(tmpbuf == NULL) {
					return;
				}
				
				/*copy to temporal buffer*/
				memcpy(tmpbuf,str+i,strlen(str)-i);
				
				/*copy temporal buffer to (str+i+strlen(b)) (str expands)*/
				memcpy(str+i+strlen(b),tmpbuf,strlen(str)-i);
				
				/*replace space left with b*/
				memcpy(str+i,b,strlen(b));
				
				if(tmpbuf != NULL) {
					free(tmpbuf);
				}
			}
		}
	}
	return;
}
