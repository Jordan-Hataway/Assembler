/*
 * tools.h
 *
 *  Created on: Nov 19, 2014
 *      Author: jordan
 */

#ifndef TOOLS_H_
#define TOOLS_H_

int growArray(int **array, int rows, int rGrowth);
void convertBaseVersion(char input, int base, char *output, int digits);
void printArray(int **array, int rows, int cols);
void decToBinary(int **word, int num, int bits);

#endif /* TOOLS_H_ */
