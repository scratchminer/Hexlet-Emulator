/* Internal header file for Hexlet's assembler */

#ifndef HEXLET_ASM_H_INTERNAL
#define HEXLET_ASM_H_INTERNAL

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_emulate.h>

/*
*  Assemble the given code string and load it into the current ROM image, returning FALSE on failure or TRUE on success.
*/
bool asm_assembleToROMImage(const char *assemblyCode);

/*
*  Get the string representing the last error from the assembler.
*/
char *asm_getError(void);

/*
*  Return the constant value parsed from the specified string. 
*  If strPart is not NULL, fill it in with a reference to the next character after the constant.
*  If noError is true, do not emit the error message (probably only useful for internal functions).
*/
s32 asm_decodeConstant(const char *number, char **strPart, bool noError);

/* alias emu_getError() so we can keep the "asm_" prefix */
char *emu_getError(void) {
	return asm_getError();
}

/* same for emu_decodeConstant() */
s32 emu_decodeConstant(char *number) {
	return asm_decodeConstant(number, NULL, FALSE);
}

#endif