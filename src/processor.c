#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processor.h"
#include "tools.h"
#include "InstructionMapping.h"

DataElement *elements;
int numEls = 0;


/**
 * Takes an empty, non-defined array, and a mips input file stripped of comments.
 * Will define the array to hold a number of 32 bit binary words that represents the
 * static data of the program
 *
 * Returns the number of rows in the data table
 */
int buildDataTable(int ***table, FILE * inputFile) {
	int rows = 0;
	typedef enum { false, true } bool;

	bool dataFlag = false;

	printf("\n\nProcessing .data section...\n");

	char *line = NULL;
	size_t len = 0;
	int read;

	while ((read = getline(&line, &len, inputFile)) != -1) {
		if (strcmp(line, ".data\n") == 0) {
			printf("begin data section\n");
			dataFlag = true;
		} else if (strcmp(line, ".text\n") == 0) {
			dataFlag = false;
		} else if (dataFlag == true) { //in data section, process data

			char * lineCopy = malloc(strlen(line) + 1);
			strcpy(lineCopy, line); //making a copy of line because strcpy modifies in place

			char * firstCopy = strtok (lineCopy, " "); //First token is the label
			char * first = malloc(strlen(firstCopy) + 1);
			strcpy(first, firstCopy);
			first[strlen(first) - 1 ] = '\0';

			char * secondCopy = strtok (NULL, " "); //second token is either .word or .asciiz
			char * second = malloc(strlen(secondCopy) + 1);
			strcpy(second, secondCopy);

			char * thirdCopy = strtok (NULL, "");  //third token is the rest
			char * third = malloc(strlen(thirdCopy) + 1);
			strcpy(third, thirdCopy);

			free(lineCopy);

			if (strcmp(second, ".word") == 0) { //.word ----------------------------------
				printf("Word Found - ");
				bool SameArrayFlag = false;
				bool VariableArrayFlag = false;
				bool valueFlag = false;

				int len = strlen(third);
				int i = 0;
				for (; i < len; i++) {
					if (third[i] == ':') {
						SameArrayFlag = true;
						break;
					} else if (third[i] == ',') {
						VariableArrayFlag = true;
						break;
					}
				}
				if (SameArrayFlag == false && VariableArrayFlag == false)
					valueFlag = true;

				if (valueFlag == true) { //SINGULAR VALUE
					printf("Singular Value\n");
					char * trash;
					int val = strtol(third, &trash, 10);
					int * word = malloc (32 * sizeof(int));
					decToBinary(&word, val, 32);
					int **string;
					string = (int**)calloc(1, sizeof(int *));
					int i = 0;
					for (; i < 1; i++)
						string[i] = (int*)calloc(32, sizeof(int));
					string[0] = word;
					addElement(first, rows * 4 + 8192);
					rows = add(table, rows, string, 1);

				} else if (SameArrayFlag == true) {
					printf("Constant-initialized Array\n");
					char * value = strtok (third, ":");
					char * number = strtok (NULL, "");
					char * trash;
					int val = strtol(value, &trash, 10); //The value
					int num = strtol(number, &trash, 10); //The number of entries
					int **string; //Make num, 32 bit memory blocks available
					string = (int**)calloc(num, sizeof(int *));
					int i = 0;
					for (; i < num; i++)
						string[i] = (int*)calloc(32, sizeof(int));
					//get binary representation of 'val'
					int * word = malloc (32 * sizeof(int));
					decToBinary(&word, val, 32);

					//for each one, set equal to the binary representation of 'val'
					i = 0;
					for (; i < num; i++) {
						string[i] = word;
					}
					addElement(first, rows * 4 + 8192);
					rows = add(table, rows, string, num);

				} else {
					printf("Variable-initialized Array\n");
					int count = 1;
					int i = 0;
					for (; i < strlen(third); i++) {
						if (third[i] == ',') {
							count++; //we now have 'count' entries
						}
					}

					int **string; //Make count, 32 bit memory blocks available
					string = (int**)calloc(count, sizeof(int *));
					i = 0;
					for (; i < count; i++)
						string[i] = (int*)calloc(32, sizeof(int));

					char * trash;
					char * value = strtok (third, ",");
					int val = strtol(value, &trash, 10);
					decToBinary(&(string[0]), val, 32);
					i = 1;
					for (; i < count; i++) {
						value = strtok (NULL, ",");
						val = strtol(value, &trash, 10);
						decToBinary(&(string[i]), val, 32);

					}

					addElement(first, rows * 4 + 8192);
					rows = add(table, rows, string, count);
				}


			} else { //.asciiz -----------------------------------------------------------
				char* output;
				int len = strlen(third);

				int numWords = (len + 4 - 1) / 4; //not (len+1) because '\0' will replace '\n'
				int **string;
				string = (int**)calloc(numWords, sizeof(int *));
				int i = 0;
				for (; i < numWords; i++)
					string[i] = (int*)calloc(32, sizeof(int)); //Allocate space for array of words representing string

				int pos = 0;
				int r = 0;
				output = malloc(1);
				while (pos <= len - 1) {
					int i = 0;
					for (; i < 4; i++, pos++) { //process string in 4 byte chunks

						if (pos == len - 1) { //Last character reached, pad to complete word
							for (; i < 4; i++) {
								int j = 0;
								for (; j < 8; j++) {
									int x = i % 4;
									int byte = 4 - x - 1;
									string[r][byte * 8 + j] = 0;
								}
							}
							pos++;
							break;
						} else {//insert into array
							convertBaseVersion(third[pos], 2, output, 8);
							int x = i % 4;
							int byte = 4 - x - 1;
							int j = 0;
							for (; j < 8; j++) {
								string[r][byte * 8 + j] = output[j] - '0';

							}
						}
					}
					r++;
				}

				addElement(first, rows * 4 + 8192);
				rows = add(table, rows, string, numWords);

				//Free string and third
				i = 0;
				for (; i < numWords; i++) {
					free(string[i]);
				}
				free(string);
				free(output);
				free(third);
			} // --------------------------------------------------------------------------
		}
	}
	return rows;
}

/**
 * Builds the array of binary instructions, the labels of instruction addresses, etc.
 *
 * Returns the number of rows (32-bit instructions) in the program
 */
int buildInstructions(int ***instructionTable, FILE * fp) {
	typedef enum { false, true } bool;

	bool textFlag = false;

	printf("\n\nProcessing .text section\n");

	char *line = NULL;
	size_t len = 0;
	int read;
	int instrCount = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		if (strcmp(line, ".data\n") == 0) {
			textFlag = false;
		} else if (strcmp(line, ".text\n") == 0) {
			printf("begin text section\n");
			textFlag = true;
		} else if (textFlag == true) { //in text section, process data

			char * lineCopy = malloc(strlen(line) + 1);
			strcpy(lineCopy, line); //making a copy of line because strcpy modifies in place

			char * firstCopy = strtok (lineCopy, " \n"); //First token is the instruction name
			char * first = malloc(strlen(firstCopy) + 1);
			strcpy(first, firstCopy);

			//Count instructions.
			int currCount = getCount(first);

			if (currCount == 0) { //LABEL
				first[strlen(first) - 1 ] = '\0'; //omit semicolon
				addElement(first, instrCount * 4);
			} else { //INSTRUCTION
				int **string;
				string = (int**)calloc(currCount, sizeof(int *));
				int i = 0;
				for (; i < currCount; i++)
					string[i] = (int*)calloc(32, sizeof(int));

				getBinaryInstruction(elements, numEls, &string, currCount, line);
				instrCount = add(instructionTable, instrCount, string, currCount);

			}

		}
	}
	return instrCount;
}


/**
 * adds the specified item to the table, enlarging the table if necessary
 * returns the number of rows in the table
 */
int add(int ***table, int rows, int **item, int words) {
	growArray(table, rows, words); //grow table

	int i = 0;
	for (; i < words; i++) {
		int j = 0;
		for (; j < 32; j++) {
			(*table)[rows + i][j] = item[i][j];
		}
	}

	return rows + words;
}

/**
 * Type = 1 for data, 0 for instruction
 */
void addElement(char *label, int address) {
	if (numEls == 0) {//no elements, must create new table
		elements = (DataElement*)malloc(1 * sizeof(DataElement));
		elements[0].label = label;
		elements[0].address = address;
		numEls = 1;
	} else {
		DataElement *temp = malloc((numEls + 1) * sizeof(DataElement));
		int i = 0;
		for (; i < numEls; i++) {
			temp[i].label = elements[i].label;
			temp[i].address = elements[i].address;
		}
		temp[numEls].label = label;
		temp[numEls].address = address;
		numEls++;

		free(elements);
		elements = temp;

	}
}

void printTable() {
	int i = 0;
	for (; i < numEls; i++) {
		printf("Address: %#.8x    Label: '%s'\n", elements[i].address, elements[i].label);
	}
}



