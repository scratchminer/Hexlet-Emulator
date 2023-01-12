/* Source file for Hexlet's assembler */


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *asm_getError(void) {
	
}

u16 asm_decode_constant(char *number) {
	
	
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
			
			return 0;
		}
		
		++arg;
	}
	else if(strchr(copy, 'x')) {
		base = 16;
	}
	else if(strchr(copy, 'b')) {
		base = 2;
	}

	/* Needed for strtol(), but unused */
	char *strPart;
	return (u16)strtol(copy, &strPart, base);
}