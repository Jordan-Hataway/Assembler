#include <stdio.h>
#include "preprocessor.h"
//Remove the comments, returns a new file
FILE* removeComments(const char * inputName, const char * outputName) {
	printf("removing comments and extra whitespace...\n");
	FILE *fp = fopen(inputName, "r");
	if (fp == NULL) {
		printf("Input file not found\n");
	   exit(-1);
	}
	FILE *new = fopen(outputName, "w+");
	

	char *line = NULL;
	size_t len = 0;
	size_t read;

	while ((read = getline(&line, &len, fp)) != -1) {
	   if (line[0] != '#' && line[0] != '\n') {
		   int i = 0;
		   while (line[i] != '\0') {
			   if (line[i] == '#') {
				   fputc('\n', new);
				   break;
			   }
			   fputc( line[i], new);
			   i++;
		   }
	   }
	}

	free(line);
	fclose(fp);
	return new;
	
}
