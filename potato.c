/*potato-c Language Parser and Lexer*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

size_t i;
size_t i2;
size_t i3;

/*Argument-processor variables*/
FILE * out;
FILE * in;
char * outputFile;
char * inputFile;
char * namePlatform;
char * functionGlossary;

const char * defaultPlatform = "x86";
const char * defaultOutputFileName = "out.bin";
unsigned long defaultFunctionGlossarySize = 1024;
unsigned long currentFunctionGlossaryPointer = 0;
unsigned long functionGlossaryMaxSize = 1024;

unsigned char platformId;
unsigned char isReg;

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

const char* token[] = {
	"/*", /*Multiline comment opening*/
	"*/", /*Multiline comment closing*/
	/*Source code-significant tokens*/
	"{",
	"}",
	"(",
	")",
	";",
	" "
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

const char* builtStatement[] = {
	"return",
	"compare",
	"copy",
	"xor",
	"or",
	"and",
	"sl",
	"sr",
	"halt"
};

const char* reg_x86_rep[] = {
	"al",
	"ax",
	"eax",
	"rax",
	"bl",
	"bx",
	"ebx",
	"rbx",
	"cl",
	"cx",
	"ecx",
	"rcx",
	"dl",
	"dx",
	"edx",
	"rdx",
	"[al]",
	"[ax]",
	"[eax]",
	"[rax]",
	"[bl]",
	"[bx]",
	"[ebx]",
	"[rbx]",
	"[cl]",
	"[cx]",
	"[ecx]",
	"[rcx]",
	"[dl]",
	"[dx]",
	"[edx]",
	"[rdx]"
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


void doStatementParse(void) {
	/*STATEMENT MINI-PROCESSOR*/
	streamEnd = 0;
	while(memcmp(fileData+i,token[7],1) == 0) { /*skip whitespaces*/
		i++;
	}
	statementLen = 0;
	while(memcmp(fileData+i,token[6],1) != 0) { /*grab all name after whitespace before ;*/
		temp_statement[statementLen] = fileData[i];
		statementLen++;
		i++;
		if(i > fileSize) {
			i = fileSize;
			streamEnd = 1;
			return;
		}
	}
	i++;
	temp_statement[statementLen] = 0;
	fprintf(stdout,"statement on scope %s : %s\n",temp,temp_statement);
	/*STATEMENT POST-PROCESSOR*/
	/*Write statement into an assembly thing*/
	/*RETURN*/
	if(memcmp(temp_statement,builtStatement[0],strlen(builtStatement[0])) == 0) {
		if(platformId == 5) {
			fprintf(out,"\trts\n");
		} else if(platformId == 10) {
			fprintf(out,"\tret\n");
		}
	}
	/*HALT*/
	else if(memcmp(temp_statement,builtStatement[8],strlen(builtStatement[8])) == 0) {
		if(platformId == 5) {
			fprintf(out,"_haltDefunct:\n");
			fprintf(out,"\tjmp _haltDefunct\n");
		} else if(platformId == 10) {
			fprintf(out,"\thlt\n");
		}
	}
	/*COPY*/
	else if(memcmp(temp_statement,builtStatement[2],strlen(builtStatement[2])) == 0) {
		/*get operands A and B*/
		/*operand A*/
		statementSecondLen = 1;
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[2])+statementSecondLen] != ',') {
			temp_operand[0][statementSecondLenSecond] = temp_statement[strlen(builtStatement[2])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[0][statementSecondLenSecond] = '\0';
				
		statementSecondLen++;
			
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[2])+statementSecondLen] != ')') {
			temp_operand[1][statementSecondLenSecond] = temp_statement[strlen(builtStatement[2])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[1][statementSecondLenSecond] = '\0';	
		fprintf(stdout,"\t%s %s\n",temp_operand[0],temp_operand[1]);
		/*Check for registers*/
		if(platformId == 10) {
			fprintf(out,"\tmov ");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[0],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[0]);
			}
			isReg = 0;
			fprintf(out,",");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[1],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[1]);
			}
			isReg = 0;
		
			fprintf(out,"\n");
		}
	}
	/*COMPARE*/
	else if(memcmp(temp_statement,builtStatement[1],strlen(builtStatement[1])) == 0) {
		/*get operands A and B*/
		/*operand A*/
		statementSecondLen = 1;
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[2])+statementSecondLen] != ',') {
			temp_operand[0][statementSecondLenSecond] = temp_statement[strlen(builtStatement[2])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[0][statementSecondLenSecond] = '\0';
			
		statementSecondLen++;
		
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[2])+statementSecondLen] != ')') {
			temp_operand[1][statementSecondLenSecond] = temp_statement[strlen(builtStatement[2])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[1][statementSecondLenSecond] = '\0';
		
		fprintf(stdout,"\t%s %s\n",temp_operand[0],temp_operand[1]);
			
		/*Check for registers*/
		if(platformId == 10) {
			fprintf(out,"\tcmp ");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[0],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[0]);
			}
			isReg = 0;
			fprintf(out,",");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[1],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[1]);
			}
			isReg = 0;
			
			fprintf(out,"\n");
		}
	}
	/*OR*/
	else if(memcmp(temp_statement,builtStatement[4],strlen(builtStatement[4])) == 0) {
		/*get operands A and B*/
		/*operand A*/
		statementSecondLen = 1;
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[4])+statementSecondLen] != ',') {
			temp_operand[0][statementSecondLenSecond] = temp_statement[strlen(builtStatement[4])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[0][statementSecondLenSecond] = '\0';
		
		statementSecondLen++;
		
		statementSecondLenSecond = 0;
		while(temp_statement[strlen(builtStatement[4])+statementSecondLen] != ')') {
			temp_operand[1][statementSecondLenSecond] = temp_statement[strlen(builtStatement[4])+statementSecondLen]; /*skip STATEMENT and (*/
			statementSecondLen++;
			statementSecondLenSecond++;
		}
		temp_operand[1][statementSecondLenSecond] = '\0';
		
		fprintf(stdout,"\t%s %s\n",temp_operand[0],temp_operand[1]);
		
		/*Check for registers*/
		if(platformId == 10) {
			fprintf(out,"\tor ");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[0],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[0]);
			}
			isReg = 0;
			fprintf(out,",");
			for(i3 = 0; i3 < 16; i3++) {
				if(memcmp(temp_operand[1],registerName[i3],strlen(registerName[i3])) == 0) {
					fprintf(out,reg_x86_rep[i3]);
					isReg = 1;
				}
			}
			if(isReg == 0) {
				fprintf(out,"%s",temp_operand[1]);
			}
			isReg = 0;
			
			fprintf(out,"\n");
		}
	}
	/*CUSTOM FUNCTION ie: foo(bar)*/
	else {
		
	}
	return;
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
		} else if(strcmp("-fgmem",argv[i]) == 0) { /*Explicit function glossary size*/
			i++; /*Increment into next argument*/
			/*If i > argc, then issue error*/
			if(i > argc) {
				fprintf(stderr,"Function glossary memory command was issued, but no int was issued\n");
				goto end;
			}
			fprintf(stdout,"Function glossary size issued was: %s\n",argv[i]);
			if(functionGlossary != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"FUnction glossary size was already issued\n");
				goto ignoreFunctionGlossarySize;
			}
			functionGlossary = malloc(atol(argv[i])+1);
			if(functionGlossary == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit function glossary size\n");
				goto end;
			}
			functionGlossaryMaxSize = atol(argv[i])+1;
			ignoreFunctionGlossarySize:
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
	
	for(i = 0; i < 38; i++) {
		if(strcmp(platforms[i],namePlatform) == 0) {
			platformId = i;
		}
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
	
	if(functionGlossary == NULL) {
		functionGlossary = malloc(defaultFunctionGlossarySize);
		if(functionGlossary == NULL) {
			fprintf(stderr,"Cannot allocate memory for function glossary");
			goto end;
		}
		fprintf(stdout,"Using default-sized (%lu bytes) function glossary\n",defaultFunctionGlossarySize);
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
		
	/*Pre-processor*/
	/*potato-c's pre-processor job is to remove comments*/
	/*replace tabs with spaces and other "cleanasing" stuff*/
	/*Replace tabs with spaces*/
	for(i = 0; i < fileSize; i++) {
		if(fileData[i] == '\t') {
			fileData[i] = ' ';
		}
	}
	/*Remove multi-line comments*/
	for(i = 0; i < fileSize; i++) {
		if(memcmp(fileData+i,token[0],2) == 0) {
			commentLen = 0;
			while(memcmp(fileData+i+commentLen,token[1],2) != 0) {
				commentLen++;
			}
			commentLen += 2; /*skip the closing comnment thing*/
			memmove(fileData+i,fileData+i+commentLen,fileSize-i);
		}
	}
	
	/*Remove newlines*/
	for(i = 0; i < fileSize; i++) {
		if(fileData[i] == '\n' && i2 == 0) {
			memmove(fileData+i,fileData+i+1,fileSize-i);
		} else if(fileData[i] == '\n' && i2 != 0) {
			i2 = 0;
		}
	}
	
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
				temp[varNameLen+1] = 0;
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
			temp[varNameLen+1] = 0;
			fprintf(stdout,"%s",temp);
			
			/*Save the function in the Function Glossary along with its parameters*/
			if(currentFunctionGlossaryPointer > functionGlossaryMaxSize-strlen(temp)-strlen(temp_param)-2) {
				fprintf(stderr,"Overpassed function glossary size");
				goto end;
			}
			memcpy(functionGlossary+currentFunctionGlossaryPointer,temp,strlen(temp));
			currentFunctionGlossaryPointer += strlen(temp); /*Copy function name into glossary*/
			
			functionGlossary[currentFunctionGlossaryPointer] = ':';
			currentFunctionGlossaryPointer++; /*Add a delimitator : for recognizing functions and params*/
			
			memcpy(functionGlossary+currentFunctionGlossaryPointer,temp_param,strlen(temp_param));
			currentFunctionGlossaryPointer += strlen(temp_param); /*Copy parameters into glossary*/
			
			functionGlossary[currentFunctionGlossaryPointer] = '\n';
			currentFunctionGlossaryPointer++; /*Add a delimitator \n for separating individual functions*/
			fprintf(stdout,">\n");
			
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
				doStatementParse();
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
	fprintf(stdout,"FUNCTION GLOSSARY\n%s\nEND OF FUNCTION GLOSSARY\n",functionGlossary);
	fprintf(stdout,"---\n%s\n---\n",fileData); /*Print current state of memory*/
	
	end:
	if(in) { fclose(in); }
	if(out) { fclose(out); }
	if(temp_decompose != NULL) { free(temp_decompose); }
	if(functionGlossary != NULL) { free(functionGlossary); }
	if(inputFile != NULL) { free(inputFile); }
	if(outputFile != NULL) { free(outputFile); }
	if(fileData != NULL) { free(fileData); }
	if(namePlatform != NULL) { free(namePlatform); }
	return 0;
}
