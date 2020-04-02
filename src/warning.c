#include "warning.h"

int warnActivated[16] = { 0,0,0 };

const char * textWarning[16] = {
	"Warning: ",
	"Implicit type macro declaration defaults to a zero-defined macro\n",
	"type macro lacks any name, and thus it's ignored\n"
};

void warning(int warn, int line) {
	if(warnActivated[warn] == 1) {
		fprintf(stdout,"[%i] %s%s",line,textWarning[0],textWarning[warn]);
	}
	return;
}

void activateWarning(int warn) {
	warnActivated[warn] = (warnActivated[warn] == 0) ? 1 : 0;
	return;
}
