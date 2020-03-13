#define COMMENT_OPEN 0
#define COMMENT_CLOSE 1
#define BRACE_OPEN 2
#define BRACE_CLOSE 3
#define PAR_OPEN 4
#define PAR_CLOSE 5
#define SEMICOLON 6
#define WHITESPACE 7
#define COMMA 8

const char* token[] = {
	"/*",
	"*/",
	"{",
	"}",
	"(",
	")",
	";",
	" ",
	","
};
