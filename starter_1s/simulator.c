/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
    int numInstructionsExecuted;
} stateType;

void printState(stateType *);

void printStats(stateType *);

static inline int convertNum(int32_t);

static int endsWith(char *, char *);


int 
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state = {0};
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    if (!endsWith(argv[1], ".mc")) {
        printf("warning: machine code file does not end with .mc\n");
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }


    /* read the entire machine-code file into memory */
    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		    if (state.numMemory >= MEMORYSIZE) {
			      fprintf(stderr, "exceeded memory size\n");
			      exit(2);
		    }
		    if (sscanf(line, "%x", state.mem+state.numMemory) != 1) {
			      fprintf(stderr, "error in reading address %d\n", state.numMemory);
			      exit(2);
		    }
            printf("mem[ %d ] 0x%08X\n", state.numMemory, state.mem[state.numMemory]);
    }

    bool halt = false;
    while (!halt) {
        printState(&state);
        state.numInstructionsExecuted++;
        int instruction = state.mem[state.pc]; 
        int opcode = (instruction >> 22) & 0x7; // Extract opcode (bits 22-24)
        
        switch (opcode) {
            case 0: // add
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    int destReg = instruction & 0x7;
                    state.reg[destReg] = state.reg[regA] + state.reg[regB];
                    state.pc++;
                }
                break;
            case 1: // nor
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    int destReg = instruction & 0x7;
                    state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
                    state.pc++;
                }
                break;
            case 2: // lw
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    int offset = convertNum(instruction & 0xFFFF);
                    int address = state.reg[regA] + offset;
                    state.reg[regB] = state.mem[address];
                    state.pc++;
                }
                break;
            case 3: // sw
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    int offset = convertNum(instruction & 0xFFFF);
                    int address = state.reg[regA] + offset;
                    state.mem[address] = state.reg[regB];
                    state.pc++;
                }
                break;
            case 4: // beq
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    int offset = convertNum(instruction & 0xFFFF);
                    if (state.reg[regA] == state.reg[regB]) {
                        state.pc += offset + 1;
                    } else {
                        state.pc++;
                    }
                }
                break;
            case 5: // jalr
                {
                    int regA = (instruction >> 19) & 0x7;
                    int regB = (instruction >> 16) & 0x7;
                    state.reg[regB] = state.pc + 1;
                    state.pc = state.reg[regA];
                }
                break;
            case 6: // halt
                state.pc++;
                halt = true;
                break;
            case 7: // noop
                state.pc++;
                break;  
        }
    }
    if (halt) {
        printStats(&state);
    }
    printState(&state);


    //Your code ends here! 

    fclose(filePtr);
    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) 
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
 * print end of run statistics like in the spec. **This is not required**,
 * but is helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 * DO NOT print "@@@" or "end state" in this function
 */
void printStats(stateType *statePtr)
{
    printf("machine halted\n");
    printf("total of %d instructions executed\n", statePtr->numInstructionsExecuted);
    printf("final state of machine:\n");
}

// Returns 1 if string ends with substr, 0 otherwise
static int endsWith(char *string, char *substr)
{
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

/*
* Write any helper functions that you wish down here. 
*/
