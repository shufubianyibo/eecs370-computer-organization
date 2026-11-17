/**
 * Project 2
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

/**
 * Requires: readAndParse is non-static and unmodified from project 1a. 
 *   inFilePtr and outFilePtr must be opened. 
 *   inFilePtr must be rewound before calling this function.
 * Modifies: outFilePtr
 * Effects: Prints the correct machine code for the input file. After 
 *   reading and parsing through inFilePtr, the pointer is rewound.
 *   Most project 1a error checks are done. No undefined labels of any
 *   type are checked, and these are instead resolved to 0.
*/
/**
 * This function will be provided in an instructor object file once the
 * project 1a deadline + late days has passed.
*/
// extern void print_inst_machine_code(FILE *inFilePtr, FILE *outFilePtr);

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
static int endsWith(char *, char *);
void checkRegister(char *regStr);
void checkOffset(int offset);
struct Symbol {
    char name[MAXLINELENGTH];
    char type;      // 'D' for Defined, 'U' for Undefined
    char section;   // 'T' for Text, 'D' for Data
    int offset;     // Address relative to the start of the section
};
struct Symbol localSymbols[MAXLINELENGTH];
int localCount;
struct Symbol globalSymbols[MAXLINELENGTH];
int globalCount = 0;
struct Relocation {
    int offset;                 // The address/index of the instruction
    char instruction[10];       // The opcode (e.g., "lw", "sw", ".fill")
    char label[MAXLINELENGTH];  // The label being referenced
};

struct Relocation relocationTable[MAXLINELENGTH];
int relocationCount = 0;

int main(int argc, char **argv) {
    char *inFileStr, *outFileStr;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <object-file>\n",
            argv[0]);
        exit(1);
    }

    inFileStr = argv[1];
    outFileStr = argv[2];

    if (!endsWith(inFileStr, ".as") &&
        !endsWith(inFileStr, ".s") &&
        !endsWith(inFileStr, ".lc2k")
    ) {
        printf("warning: assembly code file does not end with .as, .s, or .lc2k\n");
    }

    if (!endsWith(outFileStr, ".obj")) {
        printf("error: object file must end with .obj\n");
        exit(1);
    }

    inFilePtr = fopen(inFileStr, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileStr);
        exit(1);
    }
    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);
    
    outFilePtr = fopen(outFileStr, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileStr);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    int textCount = 0; 
    int dataCount = 0; 
    
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (strlen(opcode) == 0 || opcode[0] == '#') continue;
        if (strncmp(opcode, "//", 2) == 0) continue;

        char currentSection;
        int currentOffset;
        
        if (!strcmp(opcode, ".fill")) {
            currentSection = 'D';
            currentOffset = dataCount;
            dataCount++;
        } else {
            currentSection = 'T';
            currentOffset = textCount;
            textCount++;
        }

        if (strlen(label) != 0) {
            // Check for duplicates
            for (int i = 0; i < globalCount; i++) {
                if (strcmp(globalSymbols[i].name, label) == 0) {
                    printf("error: duplicate label %s\n", label); exit(1);
                }
            }
            for (int i = 0; i < localCount; i++) {
                if (strcmp(localSymbols[i].name, label) == 0) {
                    printf("error: duplicate label %s\n", label); exit(1);
                }
            }
            
            if (isupper(label[0])) {
                strcpy(globalSymbols[globalCount].name, label);
                globalSymbols[globalCount].type = 'D'; 
                globalSymbols[globalCount].section = currentSection;
                globalSymbols[globalCount].offset = currentOffset;
                globalCount++;
            } else {
                strcpy(localSymbols[localCount].name, label);
                localSymbols[localCount].type = 'D'; // Locals are always defined here
                localSymbols[localCount].section = currentSection;
                localSymbols[localCount].offset = currentOffset; 
                localCount++;
            }
        }
    }

    rewind(inFilePtr);
    int relocSize = 0;
    
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (strlen(opcode) == 0 || opcode[0] == '#' || strncmp(opcode, "//", 2) == 0) continue;

        if (!strcmp(opcode, ".fill")) {
            if (!isNumber(arg0)) {
                relocSize++; 
                bool exists = false;
                for(int i=0; i<localCount; i++) { if(!strcmp(localSymbols[i].name, arg0)) exists=true; }
                for(int i=0; i<globalCount; i++) { if(!strcmp(globalSymbols[i].name, arg0)) exists=true; }
                
                if (!exists && isupper(arg0[0])) {
                     strcpy(globalSymbols[globalCount].name, arg0);
                     globalSymbols[globalCount].type = 'U';
                     globalSymbols[globalCount].section = 'U'; 
                     globalSymbols[globalCount].offset = 0;
                     globalCount++;
                }
            }
        } else {
            if ((!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) && !isNumber(arg2)) {
                relocSize++;
                bool exists = false;
                for(int i=0; i<localCount; i++) { if(!strcmp(localSymbols[i].name, arg2)) exists=true; }
                for(int i=0; i<globalCount; i++) { if(!strcmp(globalSymbols[i].name, arg2)) exists=true; }
                
                if (!exists && isupper(arg2[0])) {
                     strcpy(globalSymbols[globalCount].name, arg2);
                     globalSymbols[globalCount].type = 'U';
                     globalSymbols[globalCount].section = 'U';
                     globalSymbols[globalCount].offset = 0;
                     globalCount++;
                }
            }
        }
    }

    fprintf(outFilePtr, "%d %d %d %d\n", textCount, dataCount, globalCount, relocSize);

    rewind(inFilePtr);
    int curTextPos = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (strlen(opcode) == 0 || opcode[0] == '#' || strncmp(opcode, "//", 2) == 0
        || !strcmp(opcode, ".fill")) continue;

        int machineCode = 0;
        
        if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
            checkRegister(arg0); checkRegister(arg1); checkRegister(arg2);
            int op = 0;
            if (!strcmp(opcode, "nor")) op = 1;
            machineCode = (op << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | atoi(arg2);
        } 
        else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) {
            checkRegister(arg0); checkRegister(arg1);
            int op = (!strcmp(opcode, "lw")) ? 2 : 3;
            int offsetField = 0;

            if (isNumber(arg2)) {
                offsetField = atoi(arg2);
                checkOffset(offsetField);
            } else {
                // Add Relocation Entry
                relocationTable[relocationCount].offset = curTextPos; 
                strcpy(relocationTable[relocationCount].instruction, opcode);
                strcpy(relocationTable[relocationCount].label, arg2);
                relocationCount++;
                
                // Resolve Symbol
                bool found = false;
                int symOffset = 0;
                char symSection = ' ';
                for(int i=0; i<localCount; i++){
                    if(!strcmp(localSymbols[i].name, arg2)) {
                        symOffset = localSymbols[i].offset;
                        symSection = localSymbols[i].section; 
                        found = true; break;
                    }
                }

                if(!found) {
                    for(int i=0; i<globalCount; i++){
                        if(!strcmp(globalSymbols[i].name, arg2)) {
                            found = true; 
                            if (globalSymbols[i].type == 'D') {
                                symOffset = globalSymbols[i].offset;
                                symSection = globalSymbols[i].section; 
                            } else {
                                symOffset = 0;
                                symSection = 'U';
                            }
                            break;
                        }
                    }
                }

                if (!found) {
                    if (!isupper(arg2[0])) { printf("error: undefined local label %s\n", arg2); exit(1); }
                    offsetField = 0; 
                } else {
                    if (symSection == 'D') {
                        offsetField = symOffset + textCount; 
                    } else {
                        offsetField = symOffset;
                    }
                }
            }
            machineCode = (op << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | (offsetField & 0xFFFF);
        }
        else if (!strcmp(opcode, "beq")) {
            checkRegister(arg0); checkRegister(arg1);
            int op = 4;
            int offsetField = 0;
            
            if (isNumber(arg2)) {
                offsetField = atoi(arg2);
            } else {
                int targetAddr = 0;
                bool found = false;
                char targetSection = ' ';

                // Find symbol
                for (int i = 0; i < localCount; ++i) {
                    if (strcmp(localSymbols[i].name, arg2) == 0) {
                        targetAddr = localSymbols[i].offset;
                        targetSection = localSymbols[i].section;
                        found = true; break;
                    }   
                }
                if (!found) {
                    for (int i = 0; i < globalCount; ++i) {
                        if (strcmp(globalSymbols[i].name, arg2) == 0 && globalSymbols[i].type == 'D') {
                            targetAddr = globalSymbols[i].offset;
                            targetSection = globalSymbols[i].section;
                            found = true; break;
                        }
                    }
                }
                
                if (!found) { printf("error: beq undefined label %s\n", arg2); exit(1); }
                
                int absoluteTarget = (targetSection == 'T') ? targetAddr : (textCount + targetAddr);
                offsetField = absoluteTarget - (curTextPos + 1);
            }
            checkOffset(offsetField);
            machineCode = (op << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16) | (offsetField & 0xFFFF);
        }
        else if (!strcmp(opcode, "jalr")) {
             checkRegister(arg0); checkRegister(arg1);
             machineCode = (5 << 22) | (atoi(arg0) << 19) | (atoi(arg1) << 16);
        }
        else if (!strcmp(opcode, "halt")) machineCode = (6 << 22);
        else if (!strcmp(opcode, "noop")) machineCode = (7 << 22);
        else { printf("error: unknown opcode %s\n", opcode); exit(1); }

        printHexToFile(outFilePtr, machineCode);
        curTextPos++;
    }

    rewind(inFilePtr);
    int curDataPos = 0;
    
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        
        if (strcmp(opcode, ".fill")) continue; 
        if (strlen(opcode) == 0 || opcode[0] == '#' 
        || strncmp(opcode, "//", 2) == 0) continue; 

        int value = 0;
        if (isNumber(arg0)) {
            value = atoi(arg0);
        } else {
            relocationTable[relocationCount].offset = curDataPos;
            strcpy(relocationTable[relocationCount].instruction, opcode);
            strcpy(relocationTable[relocationCount].label, arg0);
            relocationCount++;

            bool found = false;
            // Check Locals
            for(int i=0; i<localCount; i++){
                 if(!strcmp(localSymbols[i].name, arg0)) {
                     value = localSymbols[i].offset;
                     if (localSymbols[i].section == 'D') value += textCount;
                     found = true; break;
                 }
            }
            // Check Globals
            if(!found) {
                for(int i=0; i<globalCount; i++){
                    if(!strcmp(globalSymbols[i].name, arg0)) {
                        found = true; 
                        if (globalSymbols[i].type == 'D') {
                            value = globalSymbols[i].offset;
                            if (globalSymbols[i].section == 'D') value += textCount;
                        } else {
                            value = 0; 
                        }
                        break; 
                    }
                }
            }
            if (!found && !isupper(arg0[0])) { 
                printf("error: undefined local label %s\n", arg0); exit(1); }
        }
        printHexToFile(outFilePtr, value);
        curDataPos++;
    }

    
    for (int i = 0; i < globalCount; i++) {
        char outputSection;
        if (globalSymbols[i].type == 'U') {
            outputSection = 'U';
        } else {
            outputSection = globalSymbols[i].section; // 'T' or 'D'
        }
        fprintf(outFilePtr, "%s %c %d\n", 
            globalSymbols[i].name, 
            outputSection, 
            globalSymbols[i].offset); // Already section-relative
    }

    for (int i = 0; i < relocationCount; i++) {
        fprintf(outFilePtr, "%d %s %s\n", 
            relocationTable[i].offset, 
            relocationTable[i].instruction, 
            relocationTable[i].label);
    }



    return(0);
}

/*
* NOTE: The code defined below is not to be modifed as it is implemented correctly.
*/

// Returns non-zero if the line contains only whitespace.
int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 */
int readAndParse(FILE *inFilePtr, char *label,
	char *opcode, char *arg0, char *arg1, char *arg2) {

    char line[MAXLINELENGTH];
    char *ptr = line;

    // delete prior values
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    // read the line from the assembly-language file
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		// reached end of file
        return(0);
    }

    // check for line too long
    if (strlen(line) >= MAXLINELENGTH-1) {
		printf("error: line too long\n");
		exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    // is there a label?
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
		// successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    // Parse the rest of the line.  Would be nice to have real regular expressions, but scanf will suffice.
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}

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

void checkRegister(char *regStr) {
    if (!isNumber(regStr)) {
        printf("error: register is not an integer\n");
        exit(1);
    }
    int reg = atoi(regStr);
    if (reg < 0 || reg > 7) {
        printf("error: register out of range [0,7]\n");
        exit(1);
    }
}

void checkOffset(int offset) {
    if (offset < -32768 || offset > 32767) {
        printf("error: offset %d out of range\n", offset);
        exit(1);
    }
}

