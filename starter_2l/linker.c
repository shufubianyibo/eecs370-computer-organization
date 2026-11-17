/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

static inline void printHexToFile(FILE *, int);
static int endsWith(char *, char *);

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	unsigned int offset;
};

struct RelocationTableEntry {
    unsigned int file;
	unsigned int offset;
	char inst[6];
	char label[7];
};

struct FileData {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	unsigned int textStartingLine; // in final executable
	unsigned int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	int text[MAXSIZE * MAXFILES];
	int data[MAXSIZE * MAXFILES];
	SymbolTableEntry symbolTable[MAXSIZE * MAXFILES];
	RelocationTableEntry relocTable[MAXSIZE * MAXFILES];
};

int main(int argc, char *argv[]) {
	char *inFileStr, *outFileStr;
	FILE *inFilePtr, *outFilePtr; 
	unsigned int i, j;

    if (argc <= 2 || argc > 8 ) {
        printf("error: usage: %s <MAIN-object-file> ... <object-file> ... <machine-code-file>, with at most 5 object files\n",
				argv[0]);
		exit(1);
	}

	outFileStr = argv[argc - 1];
    if (endsWith(outFileStr, ".obj")) {
        printf("error: no machine code file given\n");
        exit(1);
    }

	outFilePtr = fopen(outFileStr, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileStr);
		exit(1);
	}

	FileData files[MAXFILES];

	// read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; ++i) {
		inFileStr = argv[i+1];

		inFilePtr = fopen(inFileStr, "r");
		printf("opening %s\n", inFileStr);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileStr);
			exit(1);
		}

		char line[MAXLINELENGTH];
		unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&textSize, &dataSize, &symbolTableSize, &relocationTableSize);

		files[i].textSize = textSize;
		files[i].dataSize = dataSize;
		files[i].symbolTableSize = symbolTableSize;
		files[i].relocationTableSize = relocationTableSize;

		// read in text section
		int instr;
		for (j = 0; j < textSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = strtol(line, NULL, 0);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < dataSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = strtol(line, NULL, 0);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		unsigned int addr;
		for (j = 0; j < symbolTableSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < relocationTableSize; ++j) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!
	int currentTextLine = 0;
	int currentDataLine = 0;
	int totalText = 0;
	int totalData = 0;
	for (i = 0; i < argc - 2; ++i) {
		totalText += files[i].textSize;
	}
	for (i = 0; i < argc - 2; ++i) {
		totalData += files[i].dataSize;
	}

	typedef struct GlobalSymbol GlobalSymbol; 
	struct GlobalSymbol {
		char label[7]; 
		unsigned int address; 
	};

	GlobalSymbol globalSymTable[MAXSIZE * MAXFILES];
	int globalCount = 0;
	currentDataLine = totalText;
	for (i = 0; i < argc - 2; i++)
	{
		files[i].textStartingLine = currentTextLine;
		files[i].dataStartingLine = currentDataLine;
		currentTextLine += files[i].textSize;
		currentDataLine += files[i].dataSize;
	}

	for (i = 0; i < argc - 2; ++i) {
		for (j = 0; j < files[i].symbolTableSize; ++j) {
			SymbolTableEntry sym = files[i].symbolTable[j];
			if (sym.location == 'T' || sym.location == 'D') {
				if (strcmp(sym.label, "Stack") == 0) {
					printf("error: label 'Stack' cannot be defined\n");
					exit(1);
				}
				
				unsigned int absoluteAddress;
				if (sym.location == 'T') {
					absoluteAddress = files[i].textStartingLine + sym.offset;
				} else { // sym.location == 'D'
					absoluteAddress = files[i].dataStartingLine + sym.offset;
				}
				
				for (int k = 0; k < globalCount; k++) {
					if (strcmp(globalSymTable[k].label, sym.label) == 0) {
						printf("error: duplicate symbol %s defined\n", sym.label);
						exit(1);
					}
				}
				
				strcpy(globalSymTable[globalCount].label, sym.label);
				globalSymTable[globalCount].address = absoluteAddress;
				globalCount++;
			}
		}
	}
	for (i = 0; i < argc - 2; ++i) {
		for (j = 0; j < files[i].symbolTableSize; ++j) {
			SymbolTableEntry sym = files[i].symbolTable[j];
			
			if (sym.location == 'U') {
				
				if (strcmp(sym.label, "Stack") == 0) {
					continue;
				}
				
				int found = 0;
				for (int k = 0; k < globalCount; k++) {
					if (strcmp(globalSymTable[k].label, sym.label) == 0) {
						found = 1;
						break;
					}
				}
				
				if (!found) {
					printf("error: undefined symbol %s\n", sym.label);
					exit(1);
				}
			}
		}
	}

	for (i = 0; i < argc - 2; ++i) {
		for (j = 0; j < files[i].relocationTableSize; ++j) {
			int offset = files[i].relocTable[j].offset;
			char *label = files[i].relocTable[j].label;
			char *inst = files[i].relocTable[j].inst;
			
			unsigned int newAbsoluteAddress = 0;
            if (isupper(label[0])) {
				if (!strcmp("Stack", label)) {
                	newAbsoluteAddress = totalText + totalData;
				} else {
					for(int k = 0; k < globalCount; k++) { 
						if(!strcmp(globalSymTable[k].label, label)) {
							newAbsoluteAddress = globalSymTable[k].address;
							break;
						}
					}
				}
            } 
			else {
				unsigned int oldLocalAddress;
				if (!strcmp(inst, ".fill")) {
					oldLocalAddress = files[i].data[offset];
				} else {
					oldLocalAddress = files[i].text[offset] & 0x0000FFFF;
				}
				if (oldLocalAddress < files[i].textSize) {
					newAbsoluteAddress = oldLocalAddress + files[i].textStartingLine;
				} else {
					newAbsoluteAddress = (oldLocalAddress - files[i].textSize) + files[i].dataStartingLine;
				}
            }
			if (!strcmp(inst, ".fill")) {
				files[i].data[offset] = newAbsoluteAddress;
			} else {
				int oldMachineCode = files[i].text[offset];
				int clearedMachineCode = oldMachineCode & 0xFFFF0000;
				unsigned int maskedAddress = newAbsoluteAddress & 0x0000FFFF;
				int newMachineCode = clearedMachineCode | maskedAddress;
				files[i].text[offset] = newMachineCode;
			}

		}
	}

	 
	
	for (i = 0; i < argc - 2; ++i) {
		for (j = 0; j < files[i].textSize; ++j) {
			printHexToFile(outFilePtr, files[i].text[j]);
		}
	}
	for (i = 0; i < argc - 2; ++i) {
		for (j = 0; j < files[i].dataSize; ++j) {
			printHexToFile(outFilePtr, files[i].data[j]);
		}
	}

    /* here is an example of using printHexToFile. This will print a
       machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);

} // main

// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

// Returns 1 if string ends with substr, 0 otherwise
static int
endsWith(char *string, char *substr) {
    size_t stringLen = strlen(string);
    size_t substrLen = strlen(substr);
    if (stringLen < substrLen) {
        return 0; // string too short
    }
    char *stringEnd = string + stringLen - substrLen;
    if (strcmp(stringEnd, substr) == 0) {
        return 1;
    }
    return 0;
}
