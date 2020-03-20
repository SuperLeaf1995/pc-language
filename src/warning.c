#include "warning.h"

const char * textWarning[] = {
	"Warning: ",
	"Implicit define macro declaration defaults to a zero-defined macro\n",
	"Define macro lacks any name, and thus it's ignored\n"
};

void warning(int warn) {
	fprintf(stdout,"%s : %s",textWarning[0],textWarning[warn]);
	return;
}
