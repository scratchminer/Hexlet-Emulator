/* Source file for Hexlet's assembler */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_driver.h>
#include "errors.h"

#include "assembler.h"

/*
*  Internal lexer struct
*/
typedef struct {
	const char *tokenStart;
	const char *current;
	u32 lineNum;
	u32 colNum;
	bool hasNewLine;
} asm_Lexer;

/*
*  Internal struct used to build the symbol table as a linked list
*/
typedef struct asm_SymbolTableEntry {
	const char *symbol;
	u32 symbolLength;
	struct asm_SymbolTableEntry *next;
} asm_SymbolTableEntry;

typedef s8 asm_Token;
#define asm_TOKEN_ERROR		-1
#define asm_TOKEN_IDENTIFIER	0
#define asm_TOKEN_LABEL		1
#define asm_TOKEN_DIRECTIVE	2
#define asm_TOKEN_REGISTER_8	3
#define asm_TOKEN_REGISTER_16	4
#define asm_TOKEN_REGISTER_24	5
#define asm_TOKEN_PGC		6
#define asm_TOKEN_AT		7
#define asm_TOKEN_PLUS		8
#define asm_TOKEN_MINUS		9
#define asm_TOKEN_COMMA		10
#define asm_TOKEN_CONSTANT	11
#define asm_TOKEN_STRING	12

static char asm_errorString[err_MAX_ERR_SIZE];

char *asm_getError(void) {
	return asm_errorString;
}

#define asm_IS_ALPHA(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || (ch) == '_' || (ch) == '.')
#define asm_IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define asm_IS_HEX_DIGIT(ch) (((ch) >= '0' && (ch) <= '9') || ((ch) >= 'a' && (ch) <= 'f') || ((ch) >= 'A' && (ch) <= 'F'))

#define asm_IS_REGISTER_8(c0, c1, c2) (((c0) == 'L' || (c0) == 'M') && ((c1) >= '0' && (c1) <= '3') && !asm_IS_ALPHA(c2) && !asm_IS_DIGIT(c2))
#define asm_IS_REGISTER_16(c0, c1, c2) (((c0) == 'W' && ((c1) >= '0' && (c1) <= '7')) && !asm_IS_ALPHA(c2) && !asm_IS_DIGIT(c2))
#define asm_IS_REGISTER_24(c0, c1, c2) (((c0) == 'S' && (c1) == 'P') || ((c0) == 'P' && (c1) >= '0' && (c1) <= '7') && !asm_IS_ALPHA(c2) && !asm_IS_DIGIT(c2))
#define asm_IS_REGSTER(c0, c1, c2) (asm_IS_REGISTER_8(c0, c1, c2) || asm_IS_REGISTER_16(c0, c1, c2) || asm_IS_REGISTER_24(c0, c1, c2))

#define asm_IS_PGC(c0, c1, c2, c3) ((c0) == 'P' && (c1) == 'G' && (c2) == 'C' && !asm_IS_ALPHA(c3) && !asm_IS_DIGIT(c3))

/*
*  Advance the given lexer past the end of the next token and return its length, or -1 if there is an error.
*/
static asm_Token asm_getNextToken(asm_Lexer *lexer, size_t *length) {
	while (*lexer->current != '\0') {
		switch (*lexer->current) {
			case ';': {
				while (*lexer->current != '\n' && *lexer->current != '\0') {
					lexer->colNum++;
					lexer->current++;
				}
				break;
			}
			case '\n':
				lexer->hasNewLine = TRUE;
				lexer->lineNum++;
				lexer->colNum = 1;
				lexer->current++;
				break;
			case '\r':
			case ' ':
			case '\t':
				lexer->colNum++;
				lexer->current++;
				break;
			default:
				break;
		}
	}
	
	lexer->tokenStart = lexer->current;
	
	if (lexer->hasNewLine) {
		lexer->hasNewLine = FALSE;
		
		if (*lexer->current == '.') {
			lexer->current++;
			lexer->colNum++;
			
			while (asm_IS_ALPHA(*lexer->current) || asm_IS_DIGIT(*lexer->current)) {
				lexer->current++;
				lexer->colNum++;
			}
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_DIRECTIVE;
		}
		else if (asm_IS_ALPHA(*lexer->current)) {
			lexer->current++;
			lexer->colNum++;
			
			while (asm_IS_ALPHA(*lexer->current) || asm_IS_DIGIT(*lexer->current)) {
				lexer->current++;
				lexer->colNum++;
			}
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			if (*lexer->current == ':') {
				lexer->current++;
				lexer->colNum++;
				return asm_TOKEN_LABEL;
			}
			else {
				return asm_TOKEN_IDENTIFIER;
			}
		}
		else {
			return asm_TOKEN_ERROR;
		}
	}
	else {
		if (*lexer->current == '$' || *lexer->current == '%' || asm_IS_DIGIT(*lexer->current)) {
			char *strPart;
			s32 constant = asm_decodeConstant(lexer->current, &strPart, TRUE);
			
			lexer->current = strPart;
			lexer->colNum += (lexer->current - lexer->tokenStart);
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			if (!constant && errno) {
				return asm_TOKEN_ERROR;
			}
			else {
				return asm_TOKEN_CONSTANT;
			}
		}
		else if (*lexer->current == '"') {
			lexer->current++;
			lexer->colNum++;
			
			while (*lexer->current != '"') {
				if (*lexer->current == '\n') {
					lexer->current++;
					lexer->lineNum++;
					lexer->colNum = 1;
				}
				else {
					lexer->current++;
					lexer->colNum++;
				}
			}
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_STRING;
		}
		else if (asm_IS_REGISTER_8(toupper(lexer->current[0]), toupper(lexer->current[1]), toupper(lexer->current[2]))) {
			lexer->current += 2;
			lexer->colNum += 2;
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_REGISTER_8;
		}
		else if (asm_IS_REGISTER_16(toupper(lexer->current[0]), toupper(lexer->current[1]), toupper(lexer->current[2]))) {
			lexer->current += 2;
			lexer->colNum += 2;
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_REGISTER_16;
		}
		else if (asm_IS_REGISTER_24(toupper(lexer->current[0]), toupper(lexer->current[1]), toupper(lexer->current[2]))) {
			lexer->current += 2;
			lexer->colNum += 2;
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_REGISTER_24;
		}
		else if (asm_IS_PGC(toupper(lexer->current[0]), toupper(lexer->current[1]), toupper(lexer->current[2]), toupper(lexer->current[3]))) {
			lexer->current += 3;
			lexer->colNum += 3;
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_PGC;
		}
		else if (*lexer->current == '@') {
			lexer->current++;
			lexer->colNum++;
			
			return asm_TOKEN_AT;
		}
		else if (*lexer->current == '+') {
			lexer->current++;
			lexer->colNum++;
			
			return asm_TOKEN_PLUS;
		}
		else if (*lexer->current == '-') {
			lexer->current++;
			lexer->colNum++;
			
			return asm_TOKEN_MINUS;
		}
		else if (*lexer->current == ',') {
			lexer->current++;
			lexer->colNum++;
			
			return asm_TOKEN_COMMA;
		}
		else if (asm_IS_ALPHA(*lexer->current)) {
			lexer->current++;
			lexer->colNum++;
			
			while (asm_IS_ALPHA(*lexer->current) || asm_IS_DIGIT(*lexer->current)) {
				lexer->current++;
				lexer->colNum++;
			}
			
			if (length != NULL) {
				*length = lexer->current - lexer->tokenStart;
			}
			
			return asm_TOKEN_IDENTIFIER;
		}
	}
}

static u8 *asm_reallocateROM(u8 *oldROMBank, u8 *size, u32 *firstROMIndex) {
	if (size != NULL) {
		u8 newSize = *size;
		
		if (firstROMIndex != NULL) {
			newSize = (u8)((0x1000000 - *firstROMIndex) >> 16);
		}
		
		u8 *ptr = drv_reallocate(oldROMBank, (u32)(*size) << 16, (u32)newSize << 16);
		*size = newSize;
		
		if (firstROMIndex != NULL) {
			*firstROMIndex = 0x1000000 - ((u32)newSize << 16);
		}
		
		return ptr;
	}
	
	return NULL;
}

bool asm_assembleToROMImage(const char *assemblyCode) {
	snprintf(asm_errorString, err_MAX_ERR_SIZE, "");
	
	const char *title = "(No title)";
	const char *developer = "(No developer)";
	
	asm_Lexer lexer;
	lexer.current = assemblyCode;
	lexer.lineNum = 1;
	lexer.colNum = 0;
	lexer.hasNewLine = TRUE;
	
	bool hasError = FALSE;
	
	u32 instPtrs[3] = {0xff0000, 0x010000, 0x100000};
	u32 *pgc = &instPtrs[0];
	
	asm_SymbolTableEntry *symbolTable = NULL;
	asm_SymbolTableEntry *lastSymbol = NULL;
	
	u8 romSize = 0x00;
	u8 cs1Type = 0x00;
	u8 cs2Type = 0x00;
	
	u32 firstROMIndex = 0xff0000;
	
	u8 *assembledROMBank = asm_reallocateROM(NULL, &romSize, &firstROMIndex);
	
	while (*lexer.current != '\0') {
		size_t length;
		
		switch (asm_getNextToken(&lexer, &length)) {
			case asm_TOKEN_DIRECTIVE: {
				if (!strncmp(lexer.current, ".org", length) || !strncmp(lexer.current, ".ORG", length)) {
					if (asm_getNextToken(&lexer, &length) != asm_TOKEN_CONSTANT) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .ORG directive needs a literal value\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else {
						s32 constant = asm_decodeConstant(lexer.current, NULL, TRUE);
						if (!constant && errno) {
							char err[err_MAX_ERR_SIZE];
							
							snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: Error decoding literal constant for .ORG directive\n", asm_errorString, lexer.lineNum, lexer.colNum);
							strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							hasError = TRUE;
						}
						else if (constant < 0x010000 || constant > 0xffffff) {
							char err[err_MAX_ERR_SIZE];
							
							snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .ORG directive out of range (valid values are $010000-$FFFFFF)\n", asm_errorString, lexer.lineNum, lexer.colNum);
							strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							hasError = TRUE;
						}
						else {
							if (pgc == &instPtrs[0] && constant < firstROMIndex) {
								firstROMIndex = constant & 0xff0000;
								
								assembledROMBank = asm_reallocateROM(assembledROMBank, &romSize, &firstROMIndex);
							}
							
							*pgc = (u32)constant;
						}
					}
					break;
				}
				else if (!strncmp(lexer.current, ".bank", length) || !strncmp(lexer.current, ".BANK", length)) {
					if (asm_getNextToken(&lexer, &length) != asm_TOKEN_IDENTIFIER) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .BANK directive needs a bank name (either ROM, CS1, or CS2)\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else if (!strncmp(lexer.current, "rom", length) || !strncmp(lexer.current, "ROM", length)) {
						pgc = &instPtrs[0];
					}
					else if (!strncmp(lexer.current, "cs1", length) || !strncmp(lexer.current, "CS1", length)) {
						pgc = &instPtrs[1];
					}
					else if (!strncmp(lexer.current, "cs2", length) || !strncmp(lexer.current, "CS2", length)) {
						pgc = &instPtrs[2];
					}
					else {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .BANK directive bank name must be ROM, CS1, or CS2\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
				}
				else if (!strncmp(lexer.current, ".hxh_title", length) || !strncmp(lexer.current, ".HXH_TITLE", length)) {
					if (asm_getNextToken(&lexer, &length) != asm_TOKEN_STRING) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .HXH_TITLE directive needs a title string\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else if (length > 127) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: ROM title is too long (maximum length: 127 bytes)\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else {
						memcpy(&assembledROMBank[(romSize << 16) - (0x10000 - 0x9f00)], lexer.current, 127);
					}
				}
				else if (!strncmp(lexer.current, ".hxh_author", length) || !strncmp(lexer.current, ".HXH_AUTHOR", length)) {
					if (asm_getNextToken(&lexer, &length) != asm_TOKEN_STRING) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: .HXH_AUTHOR directive needs an author string\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else if (length > 95) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: ROM author is too long (maximum length: 95 bytes)\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
					}
					else {
						memcpy(&assembledROMBank[(romSize << 16) - (0x10000 - 0x9f00) + 128], lexer.current, 95);
					}
				}
				
				break;
			}
			case asm_TOKEN_IDENTIFIER: {
				// resolve macros and opcodes here
				break;
			}
			case asm_TOKEN_LABEL: {
				// add the label to the symbol table
				break;
			}
			case asm_TOKEN_ERROR:
			default: {
				char err[err_MAX_ERR_SIZE];
				
				snprintf(err, err_MAX_ERR_SIZE, "%sLine %ld, column %ld: Syntax error\n", asm_errorString, lexer.lineNum, lexer.colNum);
				strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
				hasError = TRUE;
				break;
			}
		}
	}
	
	return !hasError;
}

s32 asm_decodeConstant(const char *number, char **strPart, bool noError) {
	if (!noError) {
		snprintf(asm_errorString, err_MAX_ERR_SIZE, "");
	}
	
	const char *copy = number;
	u8 base = 10;
	
	/* Make sure the prefix is stripped from the number, then predict its base */
	if (!asm_IS_HEX_DIGIT(*copy)) {
		switch(*copy) {
			case '%':
				base = 2;
				break;
			case '$':
				base = 16;
				break;
			default: {
				if (!noError) {
					snprintf(asm_errorString, err_MAX_ERR_SIZE, "Error decoding base prefix: '%c'", *copy);
				}
				return 0;
			}
		}
		copy++;
	}
	
	/* Check for errors */
	errno = 0;
	s32 ret = (s32)strtol(copy, strPart, base);
	
	if (errno && !noError) snprintf(asm_errorString, err_MAX_ERR_SIZE, "Error decoding number with base %i: '%s'", base, number);
	return ret;
}

#undef asm_IS_ALPHA
#undef asm_IS_DIGIT
#undef asm_IS_HEX_DIGIT

#undef asm_IS_REGISTER_8
#undef asm_IS_REGISTER_16
#undef asm_IS_REGISTER_24
#undef asm_IS_REGISTER

#undef asm_IS_PGC
