/*potato-c parser*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "parser.c"

int i;
int i2;
int i3;

int fileSize;

FILE * out;
FILE * in;
char * outputFile;
char * inputFile;
char * namePlatform;

const char * defaultPlatform = "x86";
const char * defaultOutputFileName = "out.bin";

unsigned char platformId;
unsigned char streamEnd;
char * fileData;

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
		fprintf(stderr,"Not engough arguments supplied\n"); goto end; }
	if(argc > 64) { /*Do not allow many arguments*/
		fprintf(stderr,"Too many arguments supplied\n"); goto end; }
	for(i = 1; i < argc; i++) {
		if(strcmp("-Wall",argv[i]) == 0) { /*Is -Wall supplied?*/
			fprintf(stdout,"All warnings enabled\n");
		} else if(strcmp("-ha",argv[i]) == 0) { /*Is -Wall supplied?*/
			fprintf(stdout,"Help - Architectures\nNote: Any architecture instruction can be written manually using the echo() command\n");
		} else if(strcmp("-p",argv[i]) == 0) { /*Explicit platform*/
			i++; /*Increment into next argument*/
			/*If i > argc, then issue error*/
			if(i > argc) {
				fprintf(stderr,"Platform command was issued, but no platform was issued\n"); goto end; }
			fprintf(stdout,"Platform issued was: %s\n",argv[i]);
			if(namePlatform != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Platform was already issued\n"); goto ignorePlatform; }
			namePlatform = malloc(strlen(argv[i])+1);
			if(namePlatform == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit platform\n"); goto end; }
			memcpy(namePlatform,argv[i],strlen(argv[i])+1);
			ignorePlatform: i = i;
		} else if(strcmp("-o",argv[i]) == 0) { /*Explicit output filename*/
			i++; /*Increment into next argument*/
			/*If i > argc, then issue error*/
			if(i > argc) {
				fprintf(stderr,"Output command was issued, but no file was issued\n"); goto end; }
			fprintf(stdout,"Output file issued was: %s\n",argv[i]);
			if(outputFile != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Output file was already issued\n"); goto ignoreOutputFile; }
			outputFile = malloc(strlen(argv[i])+1);
			if(outputFile == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit output filename\n"); goto end; }
			memcpy(outputFile,argv[i],strlen(argv[i])+1);
			ignoreOutputFile: i = i;
		} else { /*input filename (implicit)*/
			fprintf(stdout,"Input file issued was: %s\n",argv[i]);
			if(inputFile != NULL) { /*If file was already issued, skip it!*/
				fprintf(stdout,"Input file was already issued\n"); goto ignoreInputFile; }
			inputFile = malloc(strlen(argv[i])+1);
			if(inputFile == NULL) {
				fprintf(stderr,"Cannot allocate memory for explicit input filename\n"); goto end; }
			memcpy(inputFile,argv[i],strlen(argv[i])+1);
			ignoreInputFile:
			i = i;
		}
	}
	if(namePlatform == NULL) { /*No file was issued*/
		namePlatform = malloc(strlen(defaultPlatform)+1);
		if(namePlatform == NULL) {
			fprintf(stderr,"Cannot allocate memory for implicit platform\n"); goto end; }
		memcpy(namePlatform,defaultPlatform,strlen(defaultPlatform)+1);
		fprintf(stdout,"Using implicit platform: %s\n",namePlatform);
	} if(outputFile == NULL) { /*No file was issued*/
		outputFile = malloc(strlen(defaultOutputFileName)+1);
		if(outputFile == NULL) {
			fprintf(stderr,"Cannot allocate memory for implicit output filename\n"); goto end; }
		memcpy(outputFile,defaultOutputFileName,strlen(defaultOutputFileName)+1);
		fprintf(stdout,"Using implicit output filename: %s\n",outputFile);
	}
	out = fopen(outputFile,"wt"); /*Open output file in binary mode*/
	if(!out) { /*Write error*/
		fprintf(stderr,"Output file cannot be written\n"); goto end; }
	in = fopen(inputFile,"rt"); /*Open input file in text mode*/
	if(!in) { /*Read error or file does not exist*/
		fprintf(stderr,"Input file cannot be read\n"); goto end; }
	for(i = 0; i < 38; i++) { if(strcmp(platforms[i],namePlatform) == 0) { platformId = i; } }

	/*File-processor*/
	/*This processor gets file size and place all data in memory*/
	fileSize = getfilelen(in); fileData = malloc(fileSize+1);
	if(fileData == NULL) {
		fprintf(stderr,"Cannot allocate memory for file data\n"); goto end; }
	fread(fileData,sizeof(char),fileSize,in); /*Place all data on fileData*/
	fileData[fileSize+1] = 0;
	
	/*Comment remover
	Removes comments from the source code and cleans the stuff*/
	remove_between(fileData,"/*","*/"); /*Remove comments*/
	remove_all(fileData,"\t"); /*Remove all tabs to cleanase code*/
	parset_debug(stdout,fileData);

	end:
	if(in) { fclose(in); } if(out) { fclose(out); }
	if(inputFile != NULL) { free(inputFile); }
	if(outputFile != NULL) { free(outputFile); }
	if(fileData != NULL) { free(fileData); }
	if(namePlatform != NULL) { free(namePlatform); }
	return 0;
}
