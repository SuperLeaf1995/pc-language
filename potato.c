//Tidal Language Parser, Lexer and Compiler
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Argument-processor variables
FILE * out;
FILE * in;
char * outputFile;
char * inputFile;
const char * defaultOutputFileName = "out.bin";
//File-processor variables
size_t fileSize;
char * fileData;
//Pre-processor variables
size_t commentLen;
//Lexer variables
size_t varNameLen;
size_t paramLen;

char temp[64];
char temp_param[64];
char * temp_decompose;

const char* token[] = {
	"/*", //Multiline comment opening
	"*/", //Multiline comment closing
	//Source code-significant tokens
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
	//Argument-processor
	//Processes the arguments and enables certain functions of the compiler
	//including warnings
	if(argc < 1) { //argc 1 is program, so the OS is faulty somehow
		fprintf(stderr,"Not engough arguments supplied\n");
		goto end;
	}
	if(argc > 255) { //Do not allow many arguments
		fprintf(stderr,"Too many arguments supplied\n");
		goto end;
	}
	
	for(i = 1; i < argc; i++) {
		if(strcmp("-Wall",argv[i]) == 0) { //Is -Wall supplied?
			fprintf(stdout,"All warnings enabled\n");
		} else if(strcmp("-o",argv[i]) == 0) { //Explicit output filename
			i++; //Increment into next argument
			//If i > argc, then issue error
			if(i > argc) {
				fprintf(stderr,"Output command was issued, but no file was issued\n");
				goto end;
			}
			fprintf(stdout,"Output file issued was: %s\n",argv[i]);
			if(outputFile != NULL) { //If file was already issued, skip it!
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
		} else { //input filename (implicit)
			fprintf(stdout,"Input file issued was: %s\n",argv[i]);
			if(inputFile != NULL) { //If file was already issued, skip it!
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
	
	if(outputFile == NULL) { //No file was issued
		outputFile = malloc(strlen(defaultOutputFileName)+1);
		if(outputFile == NULL) {
			fprintf(stderr,"Cannot allocate memory for implicit output filename\n");
			goto end;
		}
		memcpy(outputFile,defaultOutputFileName,strlen(defaultOutputFileName)+1);
		fprintf(stdout,"Using implicit output filename: %s\n",outputFile);
	}
	
	out = fopen(outputFile,"wb"); //Open output file in binary mode
	if(!out) { //Write error
		fprintf(stderr,"Output file cannot be written\n");
		goto end;
	}
	in = fopen(inputFile,"rt"); //Open input file in text mode
	if(!in) { //Read error or file does not exist
		fprintf(stderr,"Input file cannot be read\n");
		goto end;
	}
	
	//File-processor
	//This processor gets file size and place all data in memory
	fileSize = getfilelen(in);
	fileData = malloc(fileSize+1);
	if(fileData == NULL) {
		fprintf(stderr,"Cannot allocate memory for file data\n");
		goto end;
	}
	memset(fileData,0,fileSize+1);
	fread(fileData,sizeof(char),fileSize,in); //Place all data on fileData
		
	//Pre-processor
	//Tidal's pre-processor job is to remove comments
	//replace tabs with spaces and other "cleanasing" stuff
	//Replace tabs with spaces
	for(i = 0; i < fileSize; i++) {
		if(fileData[i] == '\t') {
			fileData[i] = ' ';
		}
	}
	//Remove multi-line comments
	for(i = 0; i < fileSize; i++) {
		if(memcmp(fileData+i,token[0],2) == 0) {
			commentLen = 0;
			while(memcmp(fileData+i+commentLen,token[1],2) != 0) {
				commentLen++;
			}
			commentLen += 2; //skip the */ thing
			memmove(fileData+i,fileData+i+commentLen,fileSize-i);
		}
	}
	
	//Remove newlines
	for(i = 0; i < fileSize; i++) {
		if(fileData[i] == '\n' && i2 == 0) {
			memmove(fileData+i,fileData+i+1,fileSize-i);
		} else if(fileData[i] == '\n' && i2 != 0) {
			i2 = 0;
		}
	}
	
	//Lexer
	//List tokens and keywords in the program
	for(i = 0; i < fileSize; i++) {
		if(memcmp(fileData+i,keyword[0],8) == 0) {
			fprintf(stdout,"BSS_BYTE <");
			i += 8;
			varNameLen = 0;
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
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
			while(memcmp(fileData+i,token[7],1) == 0) { //skip any whitespaces
				i++;
			}
			while(memcmp(fileData+i,token[6],1) != 0) { //get variable name
				if(memcmp(fileData+i,token[4],1) == 0) { //print function params
					fprintf(stdout,"PARAMS > (");
					i++;
					paramLen = 0;
					while(memcmp(fileData+i,token[5],1) != 0) {
						while(memcmp(fileData+i,token[7],1) == 0) { //skip whitespaces
							i++;
						}
						temp_param[paramLen] = fileData[i];
						paramLen++;
						i++;
					}
					i++; //skip the )
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
			//Decompose Function into individual childrens
			temp_decompose = strtok(temp_param,",");
			while(temp_decompose != NULL) {
				fprintf(stdout,"%s\t",temp_decompose);
				temp_decompose = strtok(NULL,",");
			}
			fprintf(stdout,"\n");
		}
	}
	
	fprintf(stdout,"---\n%s\n---\n",fileData); //Print current state of memory
	
	end:
	if(in) { fclose(in); }
	if(out) { fclose(out); }
	if(inputFile != NULL) { free(inputFile); }
	if(outputFile != NULL) { free(outputFile); }
	if(fileData != NULL) { free(fileData); }
	return 0;
}
