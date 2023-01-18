/* Internal header file for Hexlet's assembler */

#ifndef HEXLET_ASM_H_INTERNAL
#define HEXLET_ASM_H_INTERNAL

#include <hexlet_ints.h>

/*
*  Get the string representing the last error from the graphics engine.
*/
char *asm_getError(void);

/* alias emu_decodeConstant() so we can keep the "asm_" prefix */
#define asm_decodeConstant emu_decodeConstant

#endif