/* Internal header file for Hexlet's assembler */

#ifndef HEXLET_ASM_H_INTERNAL
#define HEXLET_ASM_H_INTERNAL

#include <hexlet_ints.h>

/*
*  Get the string representing the last error from the assembler
*/
char *asm_getError(void);

s32 asm_decodeConstant(char *number);

/* alias emu_getError() so we can keep the "asm_" prefix */
char *emu_getError(void) {
	return asm_getError();
}

/* same for emu_decodeConstant() */
s32 emu_decodeConstant(char *number) {
	return asm_decodeConstant(number);
}

#endif