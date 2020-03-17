/*potato-c Language Parser and Lexer*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__)
#include <unistd.h>
#include <pthread.h>
#endif

#include <string.h>
#include <memory.h>

#include "token.h"
#include "process.h"
#include "function.h"
#include "statement.h"

function functions[64];
statement statements[64];

int i;
int i2;
int i3;
unsigned char n_statements;

int fileSize;
int commentLen;
int varNameLen;
int paramLen;
int statementLen;
int statementSecondLen;
int statementSecondLenSecond;

/*Argument-processor variables*/
FILE * out;
FILE * in;
char * outputFile;
char * inputFile;
char * namePlatform;

const char * defaultPlatform = "x86";
const char * defaultOutputFileName = "out.bin";

unsigned char platformId;
unsigned char isReg;
unsigned char current_function;
unsigned char streamEnd;

/*File-processor variables*/
char * fileData;

char temp[512];
char temp_param[512];
char temp_statement[258];
char * temp_decompose;
char temp_operand[4][258];

const char * platforms[] = {
	"cdc6000",
	"system360",
	"pdp8",
	"pdp11",
	"8080",
	"6502",
	"z80",
	"vax",
	"8051",
	"6809",
	"x86",
	"680x0",
	"mips",
	"ns320xx",
	"arm",
	"thumb",
	"sparc",
	"parisc",
	"transputer",
	"dlx",
	"powerpc",
	"alpha",
	"superh",
	"arc",
	"avr",
	"m32r",
	"mmix",
	"blackfin",
	"crusoe",
	"rx",
	"itanium",
	"score",
	"avr32",
	"mico32",
	"esirisc",
	"openrisc",
	"riscv",
	"a64",
	"elbrus"
};

const char* keyword[] = {
	"bss_byte",
	"bss_word",
	"bss_dword",
	"bss_qword",
	"byte",
	"word",
	"dword",
	"qword",
	"routine"
};

size_t getfilelen(FILE * _s) {
	register size_t i = 0;
	while(fgetc(_s) != EOF) {
		i++;
	}
	fseek(_s,SEEK_SET,0);
	return i;
}

unsigned char doStatementParse(FILE * f, char * fd, int fs, unsigned char p) {
	unsigned short statement_param_len = 0;
	char t_statement_param[512];

	while(memcmp(fd+i,token_list[WHITESPACE],1) == 0) { /*Skip whitespaces*/
		i++;
		if(i > fs) {
			i = fs;
			return 1;
		}
	}

	/*Write statement into an assembly thing*/
	for(i2 = 0; i2 < n_statements; i2++) {
		if(memcmp(fd+i,statements[i2].name,strlen(statements[i2].name)) == 0) {
			if(statements[i2].n_params == 0) {
				statementToAssembly(f,&statements[i2],p,NULL);
				i += strlen(statements[i2].name);
			} else {
				while(memcmp(fd+i,token_list[PAR_OPEN],1) != 0) { /*Skip until a brace is found*/
					i++;
				} i++;
				for(i3 = 0; i3 < statements[i2].n_params-1; i3++) {
					if(i3 != statements[i2].n_params) {
						while(memcmp(fd+i,token_list[COMMA],1) != 0) { /*Skip until a brace is found*/
							t_statement_param[statement_param_len] = fd[i];
							statement_param_len++;
							i++;
							if(statement_param_len > 512) {
								fprintf(stderr,"Parameter bigger than allowed temporal buffer\n");
								abort();
							}
						}
					}
					t_statement_param[statement_param_len] = 0;
				}
				while(memcmp(fd+i,token_list[PAR_CLOSE],1) != 0) { /*Skip until a brace is found*/
					t_statement_param[statement_param_len] = fd[i];
					statement_param_len++;
					i++;
					if(statement_param_len > 255) {
						fprintf(stderr,"Parameter bigger than allowed temporal buffer\n");
						abort();
					}
					t_statement_param[statement_param_len] = 0;
				}
				fprintf(stdout,"%i : %s\n",i,t_statement_param);
				while(memcmp(fd+i,token_list[SEMICOLON],1) != 0) { /*Read until a semicolon is found*/
					i++;
				} i++;
				statementToAssembly(f,&statements[i2],p,t_statement_param);
			}
		}
	}
	return 0;
}

int main(int argc, char * argv[]) {

	current_function = 0;

	/*Argument-processor*/
	/*Processes the arguments and enables certain functions of the compiler*/
	/*including warnings*/
	if(argc < 1) { /*argc 1 is program, so the OS is faulty somehow*/
		fprintf(stderr,"Not engough arguments supplied\n");
		goto end;
	}
	if(argc > 64) { /*Do not allow many arguments*/
		fprintf(stderr,"Too many arguments supplied\n");
		goto end;
	}

	for(i = 1; i < argc; i++) {
		if(strcmp("-Wall",argv[i]) == 0) { /*Is -Wall supplied?*/
			fprintf(stdout,"All warnings enabled\n");
		} else if(strcmp("-ha",argv[i]) == 0) { /*Is -Wall supplied?*/
			fprintf(stdout,"Help - Architectures\n");
			fprintf(stdout,"6502\n");
			fprintf(stdout,"x86\n");
			fprintf(stdout,"*Note: Any architecture instruction can be written manually using the echo() command\n");
		} else if(strcmp("-p",argv[i]) == 0) { /*Explicit platform*/
			i++; /*Increment into next argument*/
			/*If i > argc, then issue error*/
			if(i > argc) {
				fprintf(stderr,"Output command was issued, but no platform was issued\n");
				goto end;
			}
			fprintf(stdout,"Platform issued was: %s\n",argv[i]);
			if(namePlatform != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Platform was already issued\n");
				goto ignorePlatform;
			}
			namePlatform = malloc(strlen(argv[i])+1);
			if(namePlatform == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit platform\n");
				goto end;
			}
			memcpy(namePlatform,argv[i],strlen(argv[i])+1);
			ignorePlatform:
			i = i;
		} else if(strcmp("-o",argv[i]) == 0) { /*Explicit output filename*/
			i++; /*Increment into next argument*/
			/*If i > argc, then issue error*/
			if(i > argc) {
				fprintf(stderr,"Output command was issued, but no file was issued\n");
				goto end;
			}
			fprintf(stdout,"Output file issued was: %s\n",argv[i]);
			if(outputFile != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Output file was already issued\n");
				goto ignoreOutputFile;
			}
			outputFile = malloc(strlen(argv[i])+1);
			if(outputFile == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit output filename\n");
				goto end;
			}
			memcpy(outputFile,argv[i],strlen(argv[i])+1);
			ignoreOutputFile:
			i = i;
		} else { /*input filename (implicit)*/
			fprintf(stdout,"Input file issued was: %s\n",argv[i]);
			if(inputFile != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Input file was already issued\n");
				goto ignoreInputFile;
			}
			inputFile = malloc(strlen(argv[i])+1);
			if(inputFile == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit input filename\n");
				goto end;
			}
			memcpy(inputFile,argv[i],strlen(argv[i])+1);
			ignoreInputFile:
			i = i;
		}
	}

	if(namePlatform == NULL) { /*No file was issued*/
		namePlatform = malloc(strlen(defaultPlatform)+1);
		if(namePlatform == NULL) {
			fprintf(stderr,"Cannot allocate memory for implicit platform\n");
			goto end;
		}
		memcpy(namePlatform,defaultPlatform,strlen(defaultPlatform)+1);
		fprintf(stdout,"Using implicit platform: %s\n",namePlatform);
	}
	if(outputFile == NULL) { /*No file was issued*/
		outputFile = malloc(strlen(defaultOutputFileName)+1);
		if(outputFile == NULL) {
			fprintf(stderr,"Cannot allocate memory for implicit output filename\n");
			goto end;
		}
		memcpy(outputFile,defaultOutputFileName,strlen(defaultOutputFileName)+1);
		fprintf(stdout,"Using implicit output filename: %s\n",outputFile);
	}
	if(temp_decompose == NULL) {
		temp_decompose = malloc(512);
		if(temp_decompose == NULL) {
			fprintf(stderr,"Cannot allocate memory for temp_decompose");
			goto end;
		}
	}
	out = fopen(outputFile,"wt"); /*Open output file in binary mode*/
	if(!out) { /*Write error*/
		fprintf(stderr,"Output file cannot be written\n");
		goto end;
	}
	in = fopen(inputFile,"rt"); /*Open input file in text mode*/
	if(!in) { /*Read error or file does not exist*/
		fprintf(stderr,"Input file cannot be read\n");
		goto end;
	}
	for(i = 0; i < 38; i++) {
		if(strcmp(platforms[i],namePlatform) == 0) {
			platformId = i;
		}
	}

	/*File-processor*/
	/*This processor gets file size and place all data in memory*/
	fileSize = getfilelen(in);
	fileData = malloc(fileSize+1);
	if(fileData == NULL) {
		fprintf(stderr,"Cannot allocate memory for file data\n");
		goto end;
	}
	memset(fileData,0,fileSize+1);
	fread(fileData,sizeof(char),fileSize,in); /*Place all data on fileData*/

	n_statements = 25;

	createStatement(&statements[0],"reboot",0);
	createStatement(&statements[1],"return",0);
	createStatement(&statements[2],"shutdown",0);
	createStatement(&statements[3],"copy",2);
	createStatement(&statements[4],"echo",1);
	createStatement(&statements[5],"halt",0);
	
	for(i = 0; i < 20; i++) {
		createStatement(&statements[i+6],platforms[i],1);
		addStatementTranslation(&statements[i+6],"*\n",i);
		statementInfo(stdout,&statements[i+6]);
	}

	addStatementTranslation(&statements[0],"xor ax, ax\nint 16h\n",4);
	addStatementTranslation(&statements[0],"xor ax, ax\nint 16h\n",10);

	addStatementTranslation(&statements[1],"ret\n",4);
	addStatementTranslation(&statements[1],"rts\n",5);
	addStatementTranslation(&statements[1],"ret\n",10);

	addStatementTranslation(&statements[3],"mov $1,$0\n",4);
	addStatementTranslation(&statements[3],"mov $1,$0\n",10);

	addStatementTranslation(&statements[4],"*\n",0);
	addStatementTranslation(&statements[4],"*\n",1);
	addStatementTranslation(&statements[4],"*\n",2);
	addStatementTranslation(&statements[4],"*\n",3);
	addStatementTranslation(&statements[4],"*\n",4);
	addStatementTranslation(&statements[4],"*\n",5);
	addStatementTranslation(&statements[4],"*\n",6);
	addStatementTranslation(&statements[4],"*\n",7);
	addStatementTranslation(&statements[4],"*\n",8);
	addStatementTranslation(&statements[4],"*\n",9);
	addStatementTranslation(&statements[4],"*\n",10);
	addStatementTranslation(&statements[4],"*\n",11);
	
	addStatementTranslation(&statements[5],"hlt\n",4);
	addStatementTranslation(&statements[5],"hlt\n",10);

	preProcess(fileSize,fileData);

	/*Lexer*/
	/*List token_lists and keywords in the program*/
	for(i = 0; i < fileSize; i++) {
		for(i3 = 0; i3 < 7; i3++) {
			if(memcmp(fileData+i,keyword[i3],strlen(keyword[i3])) == 0) {
				fprintf(stdout,"%s <",keyword[i3]);
				varNameLen = 0;
				i += strlen(keyword[i3]);
				while(memcmp(fileData+i,token_list[WHITESPACE],1) == 0) { /*skip any whitespaces*/
					i++;
				} i--;
				while(memcmp(fileData+i,token_list[SEMICOLON],1) != 0) { /*get variable name*/
					temp[varNameLen] = fileData[i];
					varNameLen++;
					i++;
				}
				temp[varNameLen] = 0;
				fprintf(stdout,"%s",temp);
				fprintf(stdout,">\n");
			}
		}
		if(memcmp(fileData+i,keyword[8],strlen(keyword[8])) == 0) { /*"routine" keyword*/
			i += strlen(keyword[8]);
			varNameLen = 0; paramLen = 0;
			while(memcmp(fileData+i,token_list[7],1) == 0) { /*skip any whitespaces*/
				i++;
			}
			while(memcmp(fileData+i,token_list[SEMICOLON],1) != 0) { /*get variable name*/
				if(memcmp(fileData+i,token_list[PAR_OPEN],1) == 0) { /*print function params*/
					i++;
					/*paramLen = 0;*/
					while(memcmp(fileData+i,token_list[PAR_CLOSE],1) != 0) {
						temp_param[paramLen] = fileData[i];
						paramLen++;
						i++;
					}
					i++; /*skip the )*/
					goto functionBody;
				}
				temp[varNameLen] = fileData[i];
				varNameLen++;
				i++;
			}
			functionBody:
			temp[varNameLen] = 0;

			createFunction(&functions[current_function],temp,temp_param);
			fprintf(stdout,"%s ( %s )\n",functions[current_function].name,functions[current_function].params);
			current_function++;

			/*Write the temp label to the out file*/
			/*this can create a small routine*/
			fprintf(out,"%s:\n",temp);

			/*We now have the function HEADER, lets parse the body and write it
			accordingly*/
			while(memcmp(fileData+i,token_list[2],1) != 0) { /*skip until the open brace*/
				i++;
				if(i >= fileSize) {
					goto streamEnded;
				}
			}
			i++;
			while(memcmp(fileData+i,token_list[3],1) != 0 && i <= fileSize) { /*parse until closing brace*/
				streamEnd = doStatementParse(out,fileData,fileSize,platformId);
				if(streamEnd == 1) {
					fprintf(stdout,"End of data\n");
					goto streamEnded;
				}
				i++;
				if(i > fileSize) {
					goto end;
				}
			}
		}
	}

	streamEnded:

	end:
	for(i = 0; i < current_function; i++) {
		deleteFunction(&functions[i]);
	}
	for(i = 0; i < n_statements; i++) {
		deleteStatement(&statements[i]);
	}
	if(in) {
		fclose(in);
	}
	if(out) {
		fclose(out);
	}
	if(temp_decompose != NULL) {
		free(temp_decompose);
	}
	if(inputFile != NULL) {
		free(inputFile);
	}
	if(outputFile != NULL) {
		free(outputFile);
	}
	if(fileData != NULL) {
		free(fileData);
	}
	if(namePlatform != NULL) {
		free(namePlatform);
	}
	return 0;
}
