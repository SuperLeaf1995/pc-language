/*potato-c Language Parser and Lexer*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "process.c"
#include "statement.c"
#include "function.c"

function functions[64];
statement statements[64];

unsigned char doStatementParse(FILE * f, char * fd, size_t fs, unsigned char p) {
	/*Statement processor*/
	unsigned short i;
	unsigned short i2;
	/*unsigned short i3;
	unsigned short statementLen;
	
	/unsigned char t_statement_param[255];
	unsigned char t_statement[255];*/
	
	while(memcmp(fd+i,token[WHITESPACE],1) == 0) { /*Skip whitespaces*/
		i++;
		if(i > fs) {
			i = fs;
			return 1;
		}
	}
	
	/*Write statement into an assembly thing*/
	for(i2 = 0; i2 < 1; i2++) {
		if(statements[i2].n_params == 0) {
			if(memcmp(fd+i,statements[i2].name,strlen(statements[i2].name)) == 0) {
				statementToAssembly(f,&statements[i2],p);
			}
		}
	}
	return 0;
}

size_t i;
size_t i2;
size_t i3;

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
unsigned char current_function = 0;
unsigned char streamEnd;

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

/*File-processor variables*/
size_t fileSize;
char * fileData;

/*Pre-processor variables*/
size_t commentLen;

/*Lexer variables*/
size_t varNameLen;
size_t paramLen;
size_t statementLen;
size_t statementSecondLen;
size_t statementSecondLenSecond;

char temp[255];
char temp_param[255];
char temp_statement[127];
char * temp_decompose;
char temp_operand[4][127];

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

const char* registerName[] = {
	/*registers*/
	"ra8",
	"ra16",
	"ra32",
	"ra64",
	"rb8",
	"rb16",
	"rb32",
	"rb64",
	"rc8",
	"rc16",
	"rc32",
	"rc64",
	"rd8",
	"rd16",
	"rd32",
	"rd64",
	
	/*addres-register*/
	"raa8",
	"raa16",
	"raa32",
	"raa64",
	"rab8",
	"rab16",
	"rab32",
	"rab64",
	"rac8",
	"rac16",
	"rac32",
	"rac64",
	"rad8",
	"rad16",
	"rad32",
	"rad64"
	
	/*address and pointer stuff*/
	"src",
	"dst",
	"pp"
};

size_t getfilelen(FILE * _s) {
	register size_t i = 0;
	while(fgetc(_s) != EOF) {
		i++;
	}
	fseek(_s,SEEK_SET,0);
	return i;
}

int main(int argc, char * argv[]) {
	
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
		temp_decompose = malloc(255);
		if(temp_decompose == NULL) {
			fprintf(stderr,"Cannot allocate memory for temp_decompose");
			goto end;
		}
	}
	out = fopen(outputFile,"wb"); /*Open output file in binary mode*/
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
		
	createStatement(&statements[0],"call",1);
	createStatement(&statements[1],"return",0);
		
	preProcess(fileSize,fileData);
	
	/*Lexer*/
	/*List tokens and keywords in the program*/
	if(platformId == 10) {
		fprintf(out,"[section .text]\n\n");
	}
	for(i = 0; i < fileSize; i++) {
		for(i3 = 0; i3 < 7; i3++) {
			if(memcmp(fileData+i,keyword[i3],strlen(keyword[i3])) == 0) {
				fprintf(stdout,"%s <",keyword[i3]);
				varNameLen = 0;
				i += strlen(keyword[i3]);
				while(memcmp(fileData+i,token[7],1) == 0) { /*skip any whitespaces*/
					i++;
				}
				while(memcmp(fileData+i,token[6],1) != 0) { /*get variable name*/
					temp[varNameLen] = fileData[i];
					varNameLen++;
					i++;
				}
				temp[varNameLen] = 0;
				fprintf(stdout,"%s",temp);
				fprintf(stdout,">\n");
			}
		}
		if(memcmp(fileData+i,keyword[8],strlen(keyword[8])) == 0) {
			fprintf(stdout,"%s <",keyword[8]);
			i += 7;
			varNameLen = 0;
			paramLen = 0;
			while(memcmp(fileData+i,token[7],1) == 0) { /*skip any whitespaces*/
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { /*get variable name*/
				if(memcmp(fileData+i,token[4],1) == 0) { /*print function params*/
					fprintf(stdout,"parameters >>> (");
					i++;
					/*paramLen = 0;*/
					while(memcmp(fileData+i,token[5],1) != 0) {
						while(memcmp(fileData+i,token[7],1) == 0) { /*skip whitespaces*/
							i++;
						}
						temp_param[paramLen] = fileData[i];
						paramLen++;
						i++;
					}
					i++; /*skip the )*/
					fprintf(stdout,"%s",temp_param);
					fprintf(stdout,") for function: ");
					goto functionBody;
				}
				temp[varNameLen] = fileData[i];
				varNameLen++;
				i++;
			}
			functionBody:
			temp[varNameLen] = 0;
			fprintf(stdout,"%s",temp);
			fprintf(stdout,">\n");
			
			createFunction(&functions[current_function],temp,temp_param);
			current_function++;
			fprintf(stdout,"%s(%s)\n",functions[current_function].name,functions[current_function].params);
			
			/*Write the temp label with the underscore to the out file*/
			/*this can create a small routine*/
			if(platformId == 5) {
				fprintf(out,"_%s:\n",temp);
			} else if(platformId == 10) {
				fprintf(out,"_%s:\n",temp);
			}
			
			/*Decompose Function into individual childrens*/
			temp_decompose = strtok(temp_param,",");
			if(temp_decompose == NULL) {
				fprintf(stdout,"\tno parameters\n");
			}
			while(temp_decompose != NULL) {
				fprintf(stdout,"%s\t",temp_decompose);
				fprintf(stdout,"\tparameter %s\n",temp_decompose);
				temp_decompose = strtok(NULL,",");
			}
			fprintf(stdout,"\n");
			
			/*We now have the function HEADER, lets parse the body and write it
			accordingly*/
			while(memcmp(fileData+i,token[2],1) != 0) { /*skip until the open brace*/
				i++;
			}
			i++;
			while(memcmp(fileData+i,token[3],1) != 0 && i <= fileSize) { /*parse until closing brace*/
				streamEnd = doStatementParse(out,fileData,fileSize,platformId);
				if(streamEnd == 1) {
					fprintf(stdout,"end of data\n");
					goto streamEnded;
				}
				i++;
				if(i > fileSize) {
					fprintf(stderr,"error: buffer overflow\n");
					goto end;
				}
			}
		}
	}
	
	streamEnded:
	fprintf(stdout,"---\n%s\n---\n",fileData); /*Print current state of memory*/
	
	end:
	for(i = 0; i < current_function; i++) {
		deleteFunction(&functions[i]);
	}
	if(in) { fclose(in); }
	if(out) { fclose(out); }
	if(temp_decompose != NULL) { free(temp_decompose); }
	if(inputFile != NULL) { free(inputFile); }
	if(outputFile != NULL) { free(outputFile); }
	if(fileData != NULL) { free(fileData); }
	if(namePlatform != NULL) { free(namePlatform); }
	return 0;
}
