#include "macro.h"
#include "strfunc.h"

void parseMacros(register char * str) {
	size_t i; char * locStr;
	for(i = 0; i < strlen(str); i++) {
		if(strncmp(str+i,"#",strlen("#")) == 0) {
			i++;
			locStr = getName(str+i);
			if(locStr == NULL) { return; }
			i += strlen(locStr);
			free(locStr); /*free memory to avoid memory leaks*/
		}
	}
	return;
}
