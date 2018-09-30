#include <stdio.h>
#include <stdlib.h>
/*
 * tools.c
 *
 *  Created on: Nov 19, 2014
 *      Author: jordan
 */

/*
 * Grows the array by rGrowth rows and returns the new height of the array (rows + rGrowth)
 */
int growArray(int ***array, int rows, int rGrowth) {
	int newRows = rows + rGrowth;
	int cols = 32;
	if (rows == 0) { //Table is empty, nothing to copy. Create new table
		*array = (int**)calloc(newRows, sizeof(int *));
		int i = 0;
		for (; i < newRows; i++)
		  (*array)[i] = (int*)calloc(cols, sizeof(int));
		//Fill with dummy values
		i = 0;
		for (; i < newRows; i++) {
			int j = 0;
		  for (; j < cols; j++)
		    (*array)[i][j] = 2;
		}
		return newRows;
	}

	int **temp;
	temp = (int**)calloc(newRows, sizeof(int *));
	int i = 0;
	for (; i < newRows; i++)
	  temp[i] = (int*)calloc(cols, sizeof(int)); //creates new temp array

	//COPY ARRAY INTO TEMP ARRAY
	int r = 0;
	for (; r < rows; r++) {
		int j = 0;
	  for (; j < cols; j++) {
	    temp[r][j] = (*array)[r][j];
	  }
	}
	for (; r < newRows; r++) {
		int j = 0;
		for (; j < cols; j++) {
			temp[r][j] = 2; //Fill the rest with dummy value '2'
		}
	}



	i = 0;
	for (; i < rows; i++) {
		free((*array)[i]);
	}
	free(*array);
	*array = (int**)malloc(newRows * sizeof(int *));
	i = 0;
	for (; i < newRows; i++)
	  (*array)[i] = (int*)malloc(32 * sizeof(int));
	//COPY TEMP INTO ARRAY
	for (r = 0; r < newRows; r++) {
		int j = 0;
	  for (; j < cols; j++) {
	    (*array)[r][j] = temp[r][j];
	  }
	}

	//free temp and return
	i = 0;
	for (; i < newRows; i++) {
		free(temp[i]);
	}
	free(temp);
	return newRows;
}

void convertBaseVersion(char input, int base, char *output, int digits)
{
   int i, remainder;
   char digitsArray[17] = "0123456789ABCDEF";

   for (i = digits; i > 0; i--)
   {
      remainder = input % base;
      input = input / base;
      output[i - 1] = digitsArray[remainder];
   }
   output[digits] = '\0';
}

void printArray(int **array, int rows, int cols) {
	int i = 0;
	for (; i < rows; i++) {
		int j = 0;
		for (; j < cols; j++) {
			printf("%d ", array[i][j]);
		}
		printf("\n");
	}
}

void decToBinary(int **word, int num, int bits) {
	int decimalno = num,i=0;
	int temp[32];
	while(decimalno>0)
	{
	   temp[i]=decimalno%2;
	   i++;
	   decimalno=decimalno/2;
	}
	int k = bits - 1;
	int j = 0;
	for(;j<= i - 1;j++, k--)
	{
		(*word)[k] = temp[j];
	}
	for (; k >= 0; k--) {
		(*word)[k] = 0;
	}

}

