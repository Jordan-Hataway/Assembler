#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "preprocessor.h"
#include "processor.h"
#include "RegMap.h"
#include "tools.h"



int main (int argc, char *argv[]) {
	int **dataTable; //dataTable
	int rows = 0;

	int **instructionTable;
	int iRows = 0;

	typedef enum {true, false} bool;
	bool symbolsFlag = false;
	char * inputName = NULL;
	char * outputName = NULL;
	const char * tempName = "withoutComments.txt";


	setbuf(stdout, NULL);
	if (argc != 3 && argc != 4) {
		printf("%d\n", argc);
		printf("invalid number of argumentsd\n");
		printf("Usage:\nassemble <input file> <output file>\n"
				"assemble -symbols <input file> <output file>\n");
		return -1;
	}
	int i = 1;
	for (; i < argc; i++) {
		if (strcmp(argv[i], "-symbols") == 0) {
			symbolsFlag = true;
		} else if (inputName == NULL){
			inputName = argv[i];
		} else if (outputName == NULL){
			outputName = argv[i];
		} else {
			printf("Usage error. Check spelling and try again.\n");
			printf("Usage:\nassemble <input file> <output file>\n"
					"assemble -symbols <input file> <output file>\n");
			exit(-1);
		}
	}



	//PREPROCESSING STAGE
	FILE *fp = removeComments(inputName, tempName);
	fclose(fp);


	//PROCESSING STAGE
	//build data.
	fp = fopen(tempName, "r");
	if (fp == NULL) {
		printf("temp file not found\n");
	   exit(-1);
	}
	rows = buildDataTable(&dataTable, fp);
	printTable();
	printArray(dataTable, rows, 32);
	fclose(fp);

	//build instructions
	fp = fopen(tempName, "r");
	if (fp == NULL) {
		printf("temp file not found\n");
	   exit(-1);
	}
	iRows = buildInstructions(&instructionTable, fp);
	printArray(instructionTable, iRows, 32);
	printTable();
	FILE *out;
	if (symbolsFlag == true) {
		printf("PRINTING SYMBOLS...\n");
	    //create file "test" if it doesn't exist and open for writing setting permissions to 777
	    int file = open(outputName, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	    //create another file handle for output
	    int current_out = dup(1);


	    if(dup2(file, 1) < 0) {
	        fprintf(stderr, "couldn't redirect output\n");
	        return 1;
	    }

		printTable();

		if (dup2(current_out, 1) < 0) {
	        fprintf(stderr, "couldn't reset output\n");
	        return 1;
	    }
		printf("Back to screen\n");


	} else {
		out = fopen(outputName, "w+");
		int i = 0;
		for (; i < iRows; i++) {
			int j = 0;
			for (; j < 32; j++) {
				fputc(instructionTable[i][j] + '0', out);
			}
			fputc('\n', out);
		}
		fputc('\n', out);
		i = 0;
		for (; i < iRows; i++) {
			int j = 0;
			for (; j < 32; j++) {
				fputc(dataTable[i][j] + '0', out);
			}
			fputc('\n', out);
		}
		fclose(out);
	}

	//END
	fclose(fp);
	return 0;
}






