int remove_between(char * str, const char* open, const char* close) {
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

int remove_all(char * str, const char* rem) {
	size_t i;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,rem,strlen(rem)) == 0) {
			memmove(str+i,str+i+strlen(rem),strlen(str)-i);
		}
	}
	return 1;
}

const char* deb_key[] = {
	"void","int","char","short","long","double","float",
	"register","stack","nstack","unsigned","signed"
};

void parset_debug(FILE * _s, char * str) {
	size_t i; size_t i2;
	for(i = 0; i < strlen(str); i++) {
		/*if(str[i] == '\t') {
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
		} else*/
		if(str[i] == '#') {
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
				}
			}
			fputc(str[i],_s);
		}
	}
	return;
}
