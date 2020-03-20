#include "warning.h"

int warnActivated[] = { 0,0,0 };

const char * textWarning[] = {
	"Warning: ",
	"Implicit define macro declaration defaults to a zero-defined macro\n",
	"Define macro lacks any name, and thus it's ignored\n"
};

void warning(int warn, int line) {
	if(warnActivated[warn] == 1) {
		fprintf(stdout,"[%i] %s%s",line,textWarning[0],textWarning[warn]);
	}
	return;
}
