/* Source file for Hexlet's assembler */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexlet_ints.h>
#include "errors.h"

#include "assembler.h"

char asm_errorString[err_MAX_ERR_SIZE];

char *asm_getError(void) {
	return asm_errorString;
}

u32 asm_decodeConstant(char *number) {
	char *copy = number;
	u8 base = 0;
	
	/* Make sure the prefix is stripped from the number, then predict its base */
	if(!isxdigit(*copy)) {
		switch(*copy) {
		case '%':
			base = 2;
			break;
		case '$':
			base = 16;
			break;
		default:
			break;
		}
		++copy;
	}
	
	/* Needed for strtol(), but unused afterward */
	char *strPart;
	
	errno = 0;
	u32 ret = (u32)strtol(copy, &strPart, base);
	
	if(errno) snprintf(asm_errorString, err_MAX_ERR_SIZE, "Error decoding number with base %i: %s", base, number);
	return ret;
}