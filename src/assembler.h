/* Internal header file for Hexlet's assembler */

#ifndef HEXLET_ASM_H_INTERNAL
#define HEXLET_ASM_H_INTERNAL

#include <hexlet_ints.h>

/*
*  Returns the last error from the assembler
*/
char *asm_getError(void);

/*
*  Decode a constant value (used elsewhere for command line argument parsing)
*/
u32 asm_decodeConstant(char *number);

#endif