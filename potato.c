/*potato-c Language Parser and Lexer*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/*Argument-processor variables*/
FILE * out;
FILE * in;
char * outputFile;
char * inputFile;
char * namePlatform;

const char * defaultPlatform = "x86";
const char * defaultOutputFileName = "out.bin";

unsigned char platformId;

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

char temp[64];
char temp_param[64];
char temp_statement[128];
char * temp_decompose;
char temp_operand[4][64];

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
	size_t i;
	size_t i2;
	size_t i3;
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
		if(memcmp(fileData+i,keyword[0],8) == 0) {
			fprintf(stdout,"BSS_BYTE <");
			i += 8;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[1],8) == 0) {
			fprintf(stdout,"BSS_WORD <");
			i += 8;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[2],9) == 0) {
			fprintf(stdout,"BSS_DWORD <");
			i += 9;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[3],9) == 0) {
			fprintf(stdout,"BSS_QWORD <");
			i += 9;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[4],4) == 0) {
			fprintf(stdout,"BYTE <");
			i += 4;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[5],4) == 0) {
			fprintf(stdout,"WORD <");
			i += 4;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[6],5) == 0) {
			fprintf(stdout,"DWORD <");
			i += 5;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[7],5) == 0) {
			fprintf(stdout,"QWORD <");
			i += 5;
			varNameLen = 0;
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
		if(memcmp(fileData+i,keyword[8],7) == 0) {
			fprintf(stdout,"ROUTINE <");
			i += 7;
			varNameLen = 0;
			while(memcmp(fileData+i,token[7],1) == 0) { /*skip any whitespaces*/
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { /*get variable name*/
				if(memcmp(fileData+i,token[4],1) == 0) { /*print function params*/
					fprintf(stdout,"PARAMS > (");
					i++;
					paramLen = 0;
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
					fprintf(stdout,") FOR FUNCTION ");
					goto functionBody;
				}
				temp[varNameLen] = fileData[i];
				varNameLen++;
				i++;
			}
			functionBody:
			temp[varNameLen+1] = 0;
			fprintf(stdout,"%s",temp);
			fprintf(stdout,">\n");
			
			/*Write the temp label with the underscore to the out file*/
			/*this can create a small routine*/
			if(platformId == 10) {
				fprintf(out,"_%s:\n",temp);
			}
			
			/*Decompose Function into individual childrens*/
			temp_decompose = strtok(temp_param,",");
			if(temp_decompose == NULL) {
				fprintf(stdout,"\t;no params\n");
			}
			while(temp_decompose != NULL) {
				fprintf(stdout,"%s\t",temp_decompose);
				fprintf(stdout,"\t;param _%s\n",temp_decompose);
				temp_decompose = strtok(NULL,",");
			}
			fprintf(stdout,"\n");
			
			/*We now have the function HEADER, lets parse the body and write it
			accordingly*/
			while(memcmp(fileData+i,token[2],1) != 0) { /*skip until the open brace*/
				i++;
			}
			i++;
			while(memcmp(fileData+i,token[3],1) != 0) { /*parse until closing brace*/
				/*STATEMENT MINI-PROCESSOR*/
				while(memcmp(fileData+i,token[7],1) == 0) { /*skip whitespaces*/
					i++;
				}
				statementLen = 0;
				while(memcmp(fileData+i,token[6],1) != 0) { /*grab all name after whitespace before ;*/
					temp_statement[statementLen] = fileData[i];
					statementLen++;
					i++;
				}
				i++;
				temp_statement[statementLen] = 0;
				fprintf(stdout,"STATEMENT ON SCOPE %s : %s\n",temp,temp_statement);
				/*STATEMENT POST-PROCESSOR*/
				/*Write statement into an assembly thing*/
				/*RETURN*/
				if(memcmp(temp_statement,builtStatement[0],strlen(builtStatement[0])) == 0) {
					if(platformId == 10) {
						fprintf(out,"\tret\n");
					}
				}
				/*HALT*/
				if(memcmp(temp_statement,builtStatement[8],strlen(builtStatement[8])) == 0) {
					if(platformId == 10) {
						fprintf(out,"\thlt\n");
					}
				}
				/*COPY*/
				if(memcmp(temp_statement,builtStatement[2],strlen(builtStatement[2])) == 0) {
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
						if(memcmp(temp_operand[0],registerName[0],strlen(registerName[0])) == 0) { /*ra8*/
							fprintf(out,"al");
						} else if(memcmp(temp_operand[0],registerName[1],strlen(registerName[1])) == 0) { /*ra16*/
							fprintf(out,"ax");
						} else if(memcmp(temp_operand[0],registerName[2],strlen(registerName[2])) == 0) { /*ra32*/
							fprintf(out,"eax");
						} else if(memcmp(temp_operand[0],registerName[3],strlen(registerName[3])) == 0) { /*ra64*/
							fprintf(out,"rax");
						} else if(memcmp(temp_operand[0],registerName[4],strlen(registerName[4])) == 0) { /*rb8*/
							fprintf(out,"bl");
						} else if(memcmp(temp_operand[0],registerName[5],strlen(registerName[5])) == 0) { /*rb16*/
							fprintf(out,"bx");
						} else if(memcmp(temp_operand[0],registerName[6],strlen(registerName[6])) == 0) { /*rb32*/
							fprintf(out,"ebx");
						} else if(memcmp(temp_operand[0],registerName[7],strlen(registerName[7])) == 0) { /*rb64*/
							fprintf(out,"rbx");
						} else if(memcmp(temp_operand[0],registerName[8],strlen(registerName[8])) == 0) { /*rc8*/
							fprintf(out,"cl");
						} else if(memcmp(temp_operand[0],registerName[9],strlen(registerName[9])) == 0) { /*rc16*/
							fprintf(out,"cx");
						} else if(memcmp(temp_operand[0],registerName[10],strlen(registerName[10])) == 0) { /*rc32*/
							fprintf(out,"ecx");
						} else if(memcmp(temp_operand[0],registerName[11],strlen(registerName[11])) == 0) { /*rc64*/
							fprintf(out,"rcx");
						} else if(memcmp(temp_operand[0],registerName[12],strlen(registerName[12])) == 0) { /*rd8*/
							fprintf(out,"dl");
						} else if(memcmp(temp_operand[0],registerName[13],strlen(registerName[13])) == 0) { /*rd16*/
							fprintf(out,"dx");
						} else if(memcmp(temp_operand[0],registerName[14],strlen(registerName[14])) == 0) { /*rd32*/
							fprintf(out,"edx");
						} else if(memcmp(temp_operand[0],registerName[15],strlen(registerName[15])) == 0) { /*rd64*/
							fprintf(out,"rdx");
						} else if(memcmp(temp_operand[0],registerName[16],strlen(registerName[16])) == 0) { /*raa16*/
							fprintf(out,"[al]");
						} else if(memcmp(temp_operand[0],registerName[17],strlen(registerName[17])) == 0) { /*raa16*/
							fprintf(out,"[ax]");
						} else if(memcmp(temp_operand[0],registerName[18],strlen(registerName[18])) == 0) { /*raa32*/
							fprintf(out,"[eax]");
						} else if(memcmp(temp_operand[0],registerName[19],strlen(registerName[19])) == 0) { /*raa64*/
							fprintf(out,"[rax]");
						}  else if(memcmp(temp_operand[0],registerName[20],strlen(registerName[20])) == 0) { /*rab8*/
							fprintf(out,"[bl]");
						} else if(memcmp(temp_operand[0],registerName[21],strlen(registerName[21])) == 0) { /*rab16*/
							fprintf(out,"[bx]");
						} else if(memcmp(temp_operand[0],registerName[22],strlen(registerName[22])) == 0) { /*rab32*/
							fprintf(out,"[ebx]");
						} else if(memcmp(temp_operand[0],registerName[23],strlen(registerName[23])) == 0) { /*rab64*/
							fprintf(out,"[rbx]");
						} else if(memcmp(temp_operand[0],registerName[24],strlen(registerName[24])) == 0) { /*rac8*/
							fprintf(out,"[cl]");
						} else if(memcmp(temp_operand[0],registerName[25],strlen(registerName[25])) == 0) { /*rac16*/
							fprintf(out,"[cx]");
						} else if(memcmp(temp_operand[0],registerName[26],strlen(registerName[26])) == 0) { /*rac32*/
							fprintf(out,"[ecx]");
						} else if(memcmp(temp_operand[0],registerName[27],strlen(registerName[27])) == 0) { /*rac64*/
							fprintf(out,"[rcx]");
						} else if(memcmp(temp_operand[0],registerName[28],strlen(registerName[28])) == 0) { /*rad8*/
							fprintf(out,"[dl]");
						} else if(memcmp(temp_operand[0],registerName[29],strlen(registerName[29])) == 0) { /*rad16*/
							fprintf(out,"[dx]");
						} else if(memcmp(temp_operand[0],registerName[30],strlen(registerName[30])) == 0) { /*rad32*/
							fprintf(out,"[edx]");
						} else if(memcmp(temp_operand[0],registerName[31],strlen(registerName[31])) == 0) { /*rad64*/
							fprintf(out,"[rdx]");
						} else {
							fprintf(out,"%s",temp_operand[0]);
						}
						fprintf(out,",");
						if(memcmp(temp_operand[1],registerName[0],strlen(registerName[0])) == 0) { /*ra8*/
							fprintf(out,"al");
						} else if(memcmp(temp_operand[1],registerName[1],strlen(registerName[1])) == 0) { /*ra16*/
							fprintf(out,"ax");
						} else if(memcmp(temp_operand[1],registerName[2],strlen(registerName[2])) == 0) { /*ra32*/
							fprintf(out,"eax");
						} else if(memcmp(temp_operand[1],registerName[3],strlen(registerName[3])) == 0) { /*ra64*/
							fprintf(out,"rax");
						} else if(memcmp(temp_operand[1],registerName[4],strlen(registerName[4])) == 0) { /*rb8*/
							fprintf(out,"bl");
						} else if(memcmp(temp_operand[1],registerName[5],strlen(registerName[5])) == 0) { /*rb16*/
							fprintf(out,"bx");
						} else if(memcmp(temp_operand[1],registerName[6],strlen(registerName[6])) == 0) { /*rb32*/
							fprintf(out,"ebx");
						} else if(memcmp(temp_operand[1],registerName[7],strlen(registerName[7])) == 0) { /*rb64*/
							fprintf(out,"rbx");
						} else if(memcmp(temp_operand[1],registerName[8],strlen(registerName[8])) == 0) { /*rc8*/
							fprintf(out,"cl");
						} else if(memcmp(temp_operand[1],registerName[9],strlen(registerName[9])) == 0) { /*rc16*/
							fprintf(out,"cx");
						} else if(memcmp(temp_operand[1],registerName[10],strlen(registerName[10])) == 0) { /*rc32*/
							fprintf(out,"ecx");
						} else if(memcmp(temp_operand[1],registerName[11],strlen(registerName[11])) == 0) { /*rc64*/
							fprintf(out,"rcx");
						} else if(memcmp(temp_operand[1],registerName[12],strlen(registerName[12])) == 0) { /*rd8*/
							fprintf(out,"dl");
						} else if(memcmp(temp_operand[1],registerName[13],strlen(registerName[13])) == 0) { /*rd16*/
							fprintf(out,"dx");
						} else if(memcmp(temp_operand[1],registerName[14],strlen(registerName[14])) == 0) { /*rd32*/
							fprintf(out,"edx");
						} else if(memcmp(temp_operand[1],registerName[15],strlen(registerName[15])) == 0) { /*rd64*/
							fprintf(out,"rdx");
						} else if(memcmp(temp_operand[1],registerName[16],strlen(registerName[16])) == 0) { /*raa16*/
							fprintf(out,"[al]");
						} else if(memcmp(temp_operand[1],registerName[17],strlen(registerName[17])) == 0) { /*raa16*/
							fprintf(out,"[ax]");
						} else if(memcmp(temp_operand[1],registerName[18],strlen(registerName[18])) == 0) { /*raa32*/
							fprintf(out,"[eax]");
						} else if(memcmp(temp_operand[1],registerName[19],strlen(registerName[19])) == 0) { /*raa64*/
							fprintf(out,"[rax]");
						}  else if(memcmp(temp_operand[1],registerName[20],strlen(registerName[20])) == 0) { /*rab8*/
							fprintf(out,"[bl]");
						} else if(memcmp(temp_operand[1],registerName[21],strlen(registerName[21])) == 0) { /*rab16*/
							fprintf(out,"[bx]");
						} else if(memcmp(temp_operand[1],registerName[22],strlen(registerName[22])) == 0) { /*rab32*/
							fprintf(out,"[ebx]");
						} else if(memcmp(temp_operand[1],registerName[23],strlen(registerName[23])) == 0) { /*rab64*/
							fprintf(out,"[rbx]");
						} else if(memcmp(temp_operand[1],registerName[24],strlen(registerName[24])) == 0) { /*rac8*/
							fprintf(out,"[cl]");
						} else if(memcmp(temp_operand[1],registerName[25],strlen(registerName[25])) == 0) { /*rac16*/
							fprintf(out,"[cx]");
						} else if(memcmp(temp_operand[1],registerName[26],strlen(registerName[26])) == 0) { /*rac32*/
							fprintf(out,"[ecx]");
						} else if(memcmp(temp_operand[1],registerName[27],strlen(registerName[27])) == 0) { /*rac64*/
							fprintf(out,"[rcx]");
						} else if(memcmp(temp_operand[1],registerName[28],strlen(registerName[28])) == 0) { /*rad8*/
							fprintf(out,"[dl]");
						} else if(memcmp(temp_operand[1],registerName[29],strlen(registerName[29])) == 0) { /*rad16*/
							fprintf(out,"[dx]");
						} else if(memcmp(temp_operand[1],registerName[30],strlen(registerName[30])) == 0) { /*rad32*/
							fprintf(out,"[edx]");
						} else if(memcmp(temp_operand[1],registerName[31],strlen(registerName[31])) == 0) { /*rad64*/
							fprintf(out,"[rdx]");
						} else {
							fprintf(out,"%s",temp_operand[1]);
						}
						
						fprintf(out,"\n");
					}
				}
				i++;
			}
		}
	}
	
	fprintf(stdout,"---\n%s\n---\n",fileData); /*Print current state of memory*/
	
	end:
	if(in) { fclose(in); }
	if(out) { fclose(out); }
	if(inputFile != NULL) { free(inputFile); }
	if(outputFile != NULL) { free(outputFile); }
	if(fileData != NULL) { free(fileData); }
	if(namePlatform != NULL) { free(namePlatform); }
	return 0;
}
