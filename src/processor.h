#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <stdio.h>


int buildDataTable(int ***table, FILE * inputFile);
int buildInstructions(int ***instructionTable, FILE * fp);
int add(int ***table, int rows, int **item, int words);
void addElement(char *label, int address);
void printTable();
typedef struct _DataElement {
	char *label;
	int address; //hex
} DataElement;

#endif /* PROCESSOR_H_ */
