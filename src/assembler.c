/* Source file for Hexlet's assembler */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <hexlet_ints.h>
#include "errors.h"

#include "assembler.h"

char asm_errorString[err_MAX_ERR_SIZE];

char *asm_getError(void) {
	return asm_errorString;
}

s32 asm_decodeConstant(char *number) {
	char *copy = number;
	u8 base = 10;
	
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
			snprintf(asm_errorString, err_MAX_ERR_SIZE, "Error decoding base prefix: '%c'", *copy);
			return 0;
		}
		++copy;
	}
	
	/* Needed for strtol(), but unused afterward */
	char *strPart;
	
	/* Check for errors */
	errno = 0;
	s32 ret = (s32)strtol(copy, &strPart, base);
	
	if(errno) snprintf(asm_errorString, err_MAX_ERR_SIZE, "Error decoding number with base %i: '%s'", base, number);
	return ret;
}