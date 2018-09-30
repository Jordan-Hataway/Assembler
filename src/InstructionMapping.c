#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RegMap.h"
#include "processor.h"
#include "tools.h"



//This is the number of instructions that the "biggest" pseudo instruction encodes for
#define P_COUNT 2 // CHANGE IF NECESSARY

/**
 * A struct that defines certain characteristics of an instruction. The name, type, and
 * opcode (or "xxxxxx" if pseudoinstruction)
 */
typedef struct _Instruction {
	char* instrName;
	enum {R, I, J, P, S} type;
	char* opcode;
	int count;
	char *shamt;
	char *funct;
} Instruction;

/**
 * table of all possible instructions, their type, and their opcodes
 */
#define ENTRIES 28
#define dShamt "22222"
#define dFunct "222222"
Instruction InstrTable[ENTRIES] = {
		{"add", R, "000000", 1, "00000", "100000"},
		{"addi", I, "001000", 1, dShamt, dFunct},
		{"addiu", I, "001001", 1, dShamt, dFunct},
		{"and", R, "000000", 1, "00000", "100100"},
		{"andi", I, "001100", 1, dShamt, dFunct},
		{"mul", R, "011100", 1, "00000", "000010"},
		{"nor", R, "000000", 1, "00000", "100111"},
		{"or", R, "000000", 1, "00000", "100101"},
		{"ori", I, "001101", 1, dShamt, dFunct},
		{"sll", S, "000000", 1, dShamt, "000000"},
		{"slt", R, "000000", 1, "00000", "101010"},
		{"slti", I, "001010", 1, dShamt, dFunct},
		{"sra", S, "000000", 1, dShamt, "000011"},
		{"sub", R, "000000", 1, "00000", "100010"},
		{"beq", I, "000100", 1, dShamt, dFunct},
		{"blez", I, "000110", 1, dShamt, dFunct},
		{"bltz", I, "000001", 1, dShamt, dFunct},
		{"bne", I, "000101", 1, dShamt, dFunct},
		{"j", J, "000010", 1, dShamt, dFunct},
		{"lui", S, "001111", 1, dShamt, dFunct},
		{"sw", I, "101011", 1, dShamt, dFunct},
		{"ble", P, "xxxxxx", 2, dShamt, dFunct},
		{"blt", P, "xxxxxx", 2, dShamt, dFunct},
		{"la", P, "xxxxxx", 1, dShamt, dFunct},
		{"li", P, "xxxxxx", 1, dShamt, dFunct},
		{"lw", P, "100011", 1, dShamt, dFunct},
		{"nop", P, "xxxxxx", 1, dShamt, dFunct},
		{"syscall", S, "xxxxxx", 1, dShamt, dFunct}
};

typedef enum { false, true } bool;

//-------------------------------------------------------------------------------------------------

/**
 * given a mips assembly instruction inst, builds an array of binary instructions in instBinary
 * elements is the array of data labels and addresses
 *
 * IMPORTANT: Must build memory first
 */
void getBinaryInstruction(DataElement *elements, int numEls, int ***instrBinary, int rows, char* instr) {
	char * line = malloc(strlen(instr + 1));
	strcpy(line, instr);

	char * firstCopy = strtok (line, " \n"); //First token is the label
	char * first = malloc(strlen(firstCopy) + 1);
	strcpy(first, firstCopy);

	int i = 0;
	for (; i < ENTRIES; i++) {
		if (strcmp(first, InstrTable[i].instrName) == 0) {
			if (InstrTable[i].type == R) { //R TYPE INSTRUCTION
				printf("R TYPE\n");
				printf("Line: %s", instr);
				char * rdCopy = strtok (NULL, ", \n");
				char * rd = malloc(strlen(rdCopy) + 1);
				strcpy(rd, rdCopy);
				printf("Rd: %s\n", rd);

				char * rsCopy = strtok (NULL, ", \n");
				char * rs = malloc(strlen(rsCopy) + 1);
				strcpy(rs, rsCopy);
				printf("Rs: %s\n", rs);

				char * rtCopy = strtok (NULL, ", \n");
				char * rt = malloc(strlen(rtCopy) + 1);
				strcpy(rt, rtCopy);
				printf("Rt: %s\n", rt);

				int k = 0;
				int j = 0;
				for (; k < 6; k++, j++) { //opcode
					(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
				}
				j = 0;
				for (; k < 11; k++, j++) { //rs
					(*instrBinary)[0][k] = regToBinary(rs)[j] - '0';
				}
				j = 0;
				for (; k < 16; k++, j++) {//rt
					(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
				}
				j = 0;
				for (; k < 21; k++, j++) {//rd
					(*instrBinary)[0][k] = regToBinary(rd)[j] - '0';
				}
				j = 0;
				for (; k < 26; k++, j++) {//shamt
					(*instrBinary)[0][k] = InstrTable[i].shamt[j] - '0';
				}
				j = 0;
				for (; k < 32; k++, j++) {//funct
					(*instrBinary)[0][k] = InstrTable[i].funct[j] - '0';
				}


			} else if (InstrTable[i].type == I) { //I TYPE INSTRUCTION
				printf("I TYPE\n");

				char * rtCopy = strtok (NULL, ", \n");
				char * rt = malloc(strlen(rtCopy) + 1);
				strcpy(rt, rtCopy);
				printf("Rt: %s\n", rt);

				char * rsCopy = strtok (NULL, ", \n");
				char * rs = malloc(strlen(rsCopy) + 1);
				strcpy(rs, rsCopy);
				printf("Rs: %s\n", rs);

				char * immCopy = strtok (NULL, ", \n");
				char * imm = malloc(strlen(immCopy) + 1);
				strcpy(imm, immCopy);
				printf("imm: %s\n", imm);

				bool label = false;
				int val;
				int x = 0;
				for (; x < numEls; x++) {
					if (strcmp(imm, elements[x].label) == 0) {
						label = true;
						val =  elements[x].address;
						break;
					}
				}
				if (label == false) {
					char * trash;
					val = strtol(imm, &trash, 10);
				}
				int * word = malloc (16 * sizeof(int));
				decToBinary(&word, val, 16);

				int k = 0;
				int j = 0;
				for (; k < 6; k++, j++) { //opcode
					(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
				}
				j = 0;
				for (; k < 11; k++, j++) { //rs
					(*instrBinary)[0][k] = regToBinary(rs)[j] - '0';
				}
				j = 0;
				for (; k < 16; k++, j++) {//rt
					(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
				}
				j = 0;
				for (; k < 32; k++, j++) {//immediate (assumed to be int)
					(*instrBinary)[0][k] = word[j];
				}

			} else if (InstrTable[i].type == J) { //J TYPE INSTRUCTION
				printf("J TYPE\n");
				char * addrCopy = strtok (NULL, ", \n");
				char * addr = malloc(strlen(addrCopy) + 1);
				strcpy(addr, addrCopy);
				printf("addr: %s\n", addr);
				int val;
				int x = 0;
				for (; x < numEls; x++) {
					if (strcmp(elements[x].label, addr) == 0) {
						val = elements[x].address;
					}
				}

				int * word = malloc (26 * sizeof(int));
				decToBinary(&word, val, 26);


				int k = 0;
				int j = 0;
				for (; k < 6; k++, j++) { //opcode
					(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
				}
				j = 0;
				for (; k < 32; k++, j++) { //address
					(*instrBinary)[0][k] = word[j];
				}

			} else if(InstrTable[i].type == S){ //SPECIAL
				printf("SPECIAL TYPE\n");
				if (strcmp("sll", first) == 0 || strcmp("srl", first) == 0) {//sll, sra-----------
					char * rdCopy = strtok (NULL, ", \n");
					char * rd = malloc(strlen(rdCopy) + 1);
					strcpy(rd, rdCopy);
					printf("Rd: %s\n", rd);

					char * rtCopy = strtok (NULL, ", \n");
					char * rt = malloc(strlen(rtCopy) + 1);
					strcpy(rt, rtCopy);
					printf("Rt: %s\n", rt);

					char * saCopy = strtok (NULL, ", \n");
					char * sa = malloc(strlen(saCopy) + 1);
					strcpy(sa, saCopy);
					printf("sa: %s\n", sa);

					char * trash;
					int val = strtol(sa, &trash, 10);
					int * word = malloc (5 * sizeof(int));
					decToBinary(&word, val, 5);

					int k = 0;
					int j = 0;
					for (; k < 6; k++, j++) { //opcode
						(*instrBinary)[0][k] = 0;
					}
					j = 0;
					for (; k < 11; k++, j++) { //rs
						(*instrBinary)[0][k] = 0;
					}
					j = 0;
					for (; k < 16; k++, j++) {//rt
						(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
					}
					j = 0;
					for (; k < 21; k++, j++) {//rd
						(*instrBinary)[0][k] = regToBinary(rd)[j] - '0';
					}
					j = 0;
					for (; k < 26; k++, j++) {//sa
						(*instrBinary)[0][k] = word[j];
					}
					if (strcmp(first, "sll") == 0) {
						int j = 0;
						for (; k < 32; k++, j++) {//funct
							(*instrBinary)[0][k] = 0;
						}
					} else {
						int j = 0;
						for (; k < 32; k++, j++) {//funct
							(*instrBinary)[0][k] = "000011"[j];
						}
					}

				} else if (strcmp("lui", first) == 0) { //lui---------------

					char * rtCopy = strtok (NULL, ", \n");
					char * rt = malloc(strlen(rtCopy) + 1);
					strcpy(rt, rtCopy);
					printf("Rt: %s\n", rt);

					char * immCopy = strtok (NULL, ", \n");
					char * imm = malloc(strlen(immCopy) + 1);
					strcpy(imm, immCopy);
					printf("imm: %s\n", imm);

					char * trash;
					int val = strtol(imm, &trash, 10);
					int * word = malloc (16 * sizeof(int));
					decToBinary(&word, val, 16);

					int k = 0;
					int j = 0;
					for (; k < 6; k++, j++) { //opcode
						(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
					}
					j = 0;
					for (; k < 11; k++, j++) { //rs
						(*instrBinary)[0][k] = 0;
					}
					j = 0;
					for (; k < 16; k++, j++) {//rt
						(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
					}
					j = 0;
					for (; k < 32; k++, j++) {//immediate (assumed to be int)
						(*instrBinary)[0][k] = word[j];//TODO CHECK IF CAN BE ADDRESS
					}

				} else { //syscall------------------------------------------
					printf("syscall\n");
					char * ans = "00000000000000000000000000001100";
					int x = 0;
					for (; x < 32; x++) {
						(*instrBinary)[0][x] = ans[x] - '0';
					}

				}//---------------------------------------------------------


			} else {//PSEUDO-INSTRUCTION
				printf("P TYPE\n");
				if (strcmp(first, "ble") == 0) {//ble --------------

				} else if (strcmp(first, "blt") == 0) {//blt --------------

				} else if (strcmp(first, "la") == 0) {//la --------------

				} else if (strcmp(first, "li") == 0) {//li --------------

				} else if (strcmp(first, "lw") == 0) {  //lw --------------
					char * rtCopy = strtok (NULL, ", \n");
					char * rt = malloc(strlen(rtCopy) + 1);
					strcpy(rt, rtCopy);
					printf("Rt: %s\n", rt);

					char * restCopy = strtok (NULL, ", \n");
					char * rest = malloc(strlen(restCopy) + 1);
					strcpy(rest, restCopy);
					printf("rest: %s\n", rest);
					bool flag = false;
					int x = 0;
					for (; x < strlen(rest); x++) {
						if (rest[x] == '(') {
							flag = true;
							break;
						}
					}

					if (flag == true) { //Full instruction
						printf("Full lw\n");
						char * offsetCopy = strtok (rest, "(");
						char * offset = malloc(strlen(offsetCopy) + 1);
						strcpy(offset, offsetCopy);
						printf("offset: %s\n", offset);

						char * baseCopy = strtok (NULL, ")");
						char * base = malloc(strlen(baseCopy) + 1);
						strcpy(base, baseCopy);
						printf("base: %s\n", base);

						bool label = false;
						int val;
						int x = 0;
						for (; x < numEls; x++) {
							if (strcmp(offset, elements[x].label) == 0) {
								label = true;
								val =  elements[x].address;
								break;
							}
						}
						if (label == false) {
							char * trash;
							val = strtol(offset, &trash, 10);
						}
						int * word = malloc (16 * sizeof(int));
						decToBinary(&word, val, 16);

						int k = 0;
						int j = 0;
						for (; k < 6; k++, j++) { //opcode
							(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
						}
						j = 0;
						for (; k < 11; k++, j++) { //base
							(*instrBinary)[0][k] = regToBinary(base)[j] - '0';
						}
						j = 0;
						for (; k < 16; k++, j++) {//rt
							(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
						}
						j = 0;
						for (; k < 32; k++, j++) {//immediate (assumed to be int)
							(*instrBinary)[0][k] = word[j];
						}

					} else {//Pseudo-lw
						printf("PSEUDO LW\n\n");
						char * base = "$zero";
						int val;
						int x = 0;
						for (; x < numEls; x++) {
							if (strcmp(rest, elements[x].label) == 0) {
								val = elements[x].address;
								break;
							}
						}
						int * word = malloc (16 * sizeof(int));
						decToBinary(&word, val, 16);

						int k = 0;
						int j = 0;
						for (; k < 6; k++, j++) { //opcode
							(*instrBinary)[0][k] = InstrTable[i].opcode[j] - '0';
						}
						j = 0;
						for (; k < 11; k++, j++) { //base
							(*instrBinary)[0][k] = regToBinary(base)[j] - '0';
						}
						j = 0;
						for (; k < 16; k++, j++) {//rt
							(*instrBinary)[0][k] = regToBinary(rt)[j] - '0';
						}
						j = 0;
						for (; k < 32; k++, j++) {//immediate (assumed to be int)
							(*instrBinary)[0][k] = word[j];
						}
					}


				} else if (strcmp(first, "nop") == 0) {//nop---------------------
					int x = 0;
					for (; x < 32; x++) {
						(*instrBinary)[0][x] = 0;
					}

				}

//				{"ble", P, "xxxxxx", 2, dShamt, dFunct},
//				{"blt", P, "xxxxxx", 2, dShamt, dFunct},
//				{"la", P, "xxxxxx", 1, dShamt, dFunct},
//				{"li", P, "xxxxxx", 1, dShamt, dFunct},
//				{"lw", P, "xxxxxx", 1, dShamt, dFunct},
//				{"nop", P, "xxxxxx", 1, dShamt, dFunct},
			}

		}
	}
}

int getCount(char *name) {
	int i = 0;
	for (; i < ENTRIES; i++) {
		if (strcmp(InstrTable[i].instrName, name) == 0) {
			return InstrTable[i].count;
		}
	}
	return 0;
}











/**
 * instrs[] must be at least as big as the maximum number of "split" instructions
 */
//void splitPseudoInstr(char* instrs[], const char* instr) {
//	char* sentinal = "X";
//	for (int i = 0; i < ENTRIES; i++) {
//		if (strcmp(instr, InstrTable[i].instrName) == 0) {
//			if (InstrTable[i].type == P) { //IS pseudoinstruction
//				getSplits(instrs, instr);
//			} else { //IS NOT pseudoinstruction
//				instrs[0] = instr; //first is the instruction, fill rest with sentinal
//				for (int x = 1; x < P_COUNT; x++) {
//					instrs[x] = sentinal;
//				}
//			}
//		}
//	}
//}

//SHOULD I BE DOING THIS??? I AM ONLY PASSING IN INSTRUCTION NAME, NOT WHOLE THING.
//Pseudo instructions depend on entire thing for decoding... What is the register, imm, etc. :(
//maybe i could change it to pass the entire instruction, add in a function to figure out the name,
//and use the same logic to detect opcode as has already been written, but return to the processor
//an array of ALL instructions decoded. (not in pseudo-form)

//void getSplits(char* instrs[], const char* instr) {
//	//PLACE INSTRUCTIONS INTO ARRAY
//}

///**
// * Returns the opcode of the given instruction.
// * Note that a pseudoinstruction will return opcode xxxxxx
// */
//char* getOpcode(const char* instr) {
//	for (int i = 0; i < ENTRIES; i++) {
//		if (strcmp(instr, InstrTable[i].instrName) == 0) {
//			return InstrTable[i].opcode;
//		}
//	}
//	return "222222";
//}


