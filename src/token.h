#ifndef TOKEN_H
#define TOKEN_H

#define COMMENT_OPEN 0
#define COMMENT_CLOSE 1
#define BRACE_OPEN 2
#define BRACE_CLOSE 3
#define PAR_OPEN 4
#define PAR_CLOSE 5
#define SEMICOLON 6
#define WHITESPACE 7
#define COMMA 8

const char* token_list[] = {
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

/* token.h
 * Includes a list of Tokens to be considered part of the syntax of
 * the programming language, each serve a porpouse*/

typedef struct token {
	char * str;
	signed char match; /* 0 = Not a match token
						* 1 = Opening token
						* 2 = Closing token*/
	signed char mgroup; /* Indicates a group where the token can
						 * Close/Open with the use of another token
						 * of the same group*/
}token;

void createToken(token * t, char * n, signed char match, signed char mgroup);
void deleteToken(token * t);

#include "token.c"

#endif
