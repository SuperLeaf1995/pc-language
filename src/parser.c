#include "parser.h"
#include "strfunc.h"

const char* deb_key[] = {
	"void","int","char","short","long","double","float",
	"register","stack","nstack","unsigned","signed"
};

void parsetDebug(FILE * _s, register char * str) {
	size_t i; size_t i2;
	for(i = 0; i < strlen(str); i++) {
		if(str[i] == '\t') {
			#if defined(__unix__)
			fputs("\033[0;35m\\t\033[0m",_s);
			#else
			fputs("\\t",_s);
			#endif
		} else if(str[i] == '\n') {
			#if defined(__unix__)
			fputs("\033[0;35m\\n\n\033[0m",_s);
			#else
			fputs("\\n\n",_s);
			#endif
		} else if(str[i] == '#') {
			while(str[i] != '\n' && i < strlen(str)) {
				#if defined(__unix__)
				fprintf(_s,"\033[0;36m%c\033[0m",str[i]);
				#else
				fprintf(_s,"%s",deb_key[i2]);
				#endif
				i++;
			} i--;
		} else {
			for(i2 = 0; i2 < 11; i2++) {
				if(strncmp(str+i,deb_key[i2],strlen(deb_key[i2])) == 0) {
					#if defined(__unix__)
					fprintf(_s,"\033[0;31m%s\033[0m",deb_key[i2]);
					#else
					fprintf(_s,"%s",deb_key[i2]);
					#endif
					i += strlen(deb_key[i2]);
					if(i > strlen(str)) { return; }
				}
			}
			fprintf(_s,"%c",str[i]);
		}
	}
	return;
}
