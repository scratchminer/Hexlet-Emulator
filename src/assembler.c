/* Source file for Hexlet's assembler */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
	size_t symbolLength;
	s32 value;
	bool resolved;
	struct asm_SymbolTableEntry *next;
} asm_SymbolTableEntry;

typedef s8 asm_Token;
#define asm_TOKEN_ERROR		-1
#define asm_TOKEN_OPCODE	0
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
#define asm_TOKEN_IDENTIFIER	12
#define asm_TOKEN_STRING	1

typedef s8 asm_OperandSize;
#define asm_SIZE_INFER		-1
#define asm_SIZE_BYTE		0
#define asm_SIZE_WORD		1
#define asm_SIZE_POINTER	2

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
				return asm_TOKEN_OPCODE;
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
		else {
			return asm_TOKEN_ERROR;
		}
	}
}

/*
*  Reallocate the memory buffer used for the ROM using drv_reallocate and return the new buffer, or NULL if there is an error.
*/
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

/*
*  Append a new symbol with the specified name onto the end of the symbol table and return it.
*/
static asm_SymbolTableEntry *asm_addSymbol(asm_SymbolTableEntry **symbolTable, asm_SymbolTableEntry *lastSymbol, const char *name, size_t nameLength) {
	asm_SymbolTableEntry *newSymbol = drv_reallocate(NULL, 0, sizeof(asm_SymbolTableEntry));
	newSymbol->symbol = name;
	newSymbol->symbolLength = nameLength;
	newSymbol->value = 0;
	newSymbol->resolved = FALSE;
	newSymbol->next = NULL;
	
	if (*symbolTable == NULL) {
		*symbolTable = newSymbol;
	}
	else {
		lastSymbol->next = newSymbol;
	}
	
	return newSymbol;
}

/*
*  Get a reference to the symbol with the specified name.
*/
static asm_SymbolTableEntry *asm_lookupSymbol(asm_SymbolTableEntry **symbolTable, const char *name, size_t nameLength) {
	asm_SymbolTableEntry *sym = NULL;
	
	if (symbolTable != NULL) {
		for (sym = *symbolTable; sym->next != NULL; sym = sym->next) {
			if (!strncmp(sym->symbol, name, (nameLength > sym->symbolLength) ? nameLength : sym->symbolLength)) {
				return sym;
			}
		}
	}
	
	return NULL;
}

/*
*  Assemble a single RM operand in the assembler source and return the value that goes into the opcode, or -1 on error.
*/
static s8 asm_assembleRMOperand(asm_Lexer *lexer, u32 *pgc, u32 firstROMIndex, u8 *assembledROMBank, asm_OperandSize size, u32 pass, bool *requiresMorePasses) {
	size_t length;
	
	switch (asm_getNextToken(lexer, &length)) {
		case asm_TOKEN_REGISTER_8: {
			switch (size) {
				case asm_SIZE_INFER:
				case asm_SIZE_BYTE: {
					s8 value = 0;
					
					if (lexer->tokenStart[0] == 'M') {
						value |= 0x10;
					}
					
					value |= (lexer->tokenStart[1] - '0') << 2;
					return value;
				}
				case asm_SIZE_WORD: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is an 8-bit register, but operand size is .W (16-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				case asm_SIZE_POINTER: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is an 8-bit register, but operand size is .P (24-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				default:
					return -1;
			}
		}
		case asm_TOKEN_REGISTER_16: {
			switch (size) {
				case asm_SIZE_BYTE: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is a 16-bit register, but operand size is .B (8-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				case asm_SIZE_INFER:
				case asm_SIZE_WORD: {
					return (lexer->tokenStart[1] - '0') << 2;
				}
				case asm_SIZE_POINTER: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is a 16-bit register, but operand size is .P (24-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				default:
					return -1;
			}
		}
		case asm_TOKEN_REGISTER_24: {
			switch (size) {
				case asm_SIZE_BYTE: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is a 24-bit register, but operand size is .B (8-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				case asm_SIZE_WORD: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register operand is a 24-bit register, but operand size is .W (16-bit)\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				case asm_SIZE_INFER:
				case asm_SIZE_POINTER: {
					if (toupper(lexer->tokenStart[0]) == 'S') {
						return 0x1c;
					}
					else {
						return (lexer->tokenStart[1] - '0') << 2;
					}
				}
				default:
					return -1;
			}
		}
		case asm_TOKEN_CONSTANT: {
			s32 constant = asm_decodeConstant(lexer->tokenStart, NULL, TRUE);
			if (!constant && errno) {
				if (pass == 0) {
					char err[err_MAX_ERR_SIZE];
				
					snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding immediate value for RM operand\n", asm_errorString, lexer->lineNum, lexer->colNum);
					strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
				}
				
				return -1;
			}
			
			if (constant > 0xffffff || constant < -0x8000) {
				if (pass == 0) {
					char err[err_MAX_ERR_SIZE];
	
					snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Immediate value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
					strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
				}
	
				return -1;
			}
			else if (constant < 0x0000 || constant > 0x0010 && constant < 0x8000) {
				assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
				assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
				
				return 0x21;
			}
			else if (constant < 0x10) {
				return 0x03 | (constant << 2);
			}
			else {
				assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
				assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
				assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
				assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
				
				return 0x25;
			}
		}
		case asm_TOKEN_IDENTIFIER: {
			asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer->tokenStart, length);
			
			if (symbol == NULL) {
				symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer->tokenStart, length);
				lastSymbol = symbol;
				
				*requiresMorePasses = TRUE;
				
				return 0x00;
			}
			else if (symbol->resolved) {
				s32 value = symbol->value;
				
				if (value > 0xffffff || value < -0x8000) {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Immediate value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
				else if (value < 0x0000 || value > 0x0010 && value < 0x8000) {
					assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
					assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
				
					return 0x21;
				}
				else if (value < 0x10) {
					return 0x03 | (value << 2);
				}
				else {
					assembledROMBank[((*pgc)++) - firstROMIndex] = value & 0xff;
					assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 8) & 0xff;
					assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 16) & 0xff;
					assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
					
					return 0x25;
				}
			}
			else {
				*requiresMorePasses = TRUE;
				
				return 0x00;
			}
		}
		case asm_TOKEN_AT: {
			switch (asm_getNextToken(lexer, &length)) {
				case asm_TOKEN_CONSTANT: {
					s32 constant = asm_decodeConstant(lexer->tokenStart, NULL, TRUE);
					if (!constant && errno) {
						if (pass == 0) {
							char err[err_MAX_ERR_SIZE];
					
							snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding absolute value for RM operand\n", asm_errorString, lexer->lineNum, lexer->colNum);
							strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						}
						
						return -1;
					}
					else if (constant > 0xffffff || constant < -0x8000) {
						if (pass == 0) {
							char err[err_MAX_ERR_SIZE];
							
							snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Absolute value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
							strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						}
						
						return -1;
					}
					
					asm_Token token = asm_getNextToken(lexer, &length);
					
					if (token == asm_TOKEN_PLUS) {
						switch (asm_getNextToken(lexer, &length)) {
							case asm_TOKEN_REGISTER_8: {
								u8 indexWord = 0x00;
								
								if (lexer->tokenStart[0] == 'M') {
									indexWord |= 0x04;
								}
								
								indexWord |= (lexer->tokenStart[1] - '0');
								
								assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
								
								return 0x3d;
							}
							case asm_TOKEN_REGISTER_16: {
								u8 indexWord = 0x40 | ((lexer->tokenStart[1] - '0') << 8);
								
								assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
								
								return 0x3d;
							}
							case asm_TOKEN_REGISTER_24: {
								u8 indexWord = 0x80;
								
								if (lexer->tokenStart[0] == 'S') {
									indexWord |= 0x07;
								}
								else {
									indexWord |= ((lexer->tokenStart[1] - '0') << 8);
								}
								
								assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
								assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
								
								return 0x3d;
							}
							case asm_TOKEN_IDENTIFIER: {
								if (toupper(lexer->tokenStart[2]) == 'S' && toupper(lexer->tokenStart[3]) == 'X' && !asm_IS_ALPHA(lexer->tokenStart[4]) && !asm_IS_DIGIT(lexer->tokenStart[4])) {
									if (asm_IS_REGISTER_8(toupper(lexer->tokenStart[0]), toupper(lexer->tokenStart[1]), ' ')) {
										u8 indexWord = 0x08;
										
										if (lexer->tokenStart[0] == 'M') {
											indexWord |= 0x04;
										}
										
										indexWord |= (lexer->tokenStart[1] - '0');
										
										assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
										
										return 0x3d;
									}
									else if (asm_IS_REGISTER_16(toupper(lexer->tokenStart[0]), toupper(lexer->tokenStart[1]), ' ')) {
										u8 indexWord = 0x48 | ((lexer->tokenStart[1] - '0') << 8);
										
										assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
										
										return 0x3d;
									}
								}
							}
							default: {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								}
								
								return -1;
							}
						}
					}
					else if (constant < 0x8000 || constant > 0xff7fff) {
						assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
						assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
						
						return 0x29;
					}
					else {
						assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
						assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
						assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
						assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
						
						return 0x2d;
					}
				}
				case asm_TOKEN_IDENTIFIER: {
					asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer->tokenStart, length);
					
					if (symbol == NULL) {
						symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer->tokenStart, length);
						lastSymbol = symbol;
						
						*requiresMorePasses = TRUE;
						
						return 0x00;
					}
					else if (symbol->resolved) {
						s32 value = symbol->value;
						s32 pgcValue = value - *pgc;
						
						if (value > 0xffffff || value < -0x8000) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
						
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Absolute value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							}
							
							return -1;
						}
						if (value < 0x8000 || value > 0xff7fff) {
							assembledROMBank[((*pgc)++) - firstROMIndex] = value & 0xff;
							assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 8) & 0xff;
							
							return 0x29;
						}
						else if (pgcValue < 0x8000 || pgcValue > 0xff7fff) {
							assembledROMBank[((*pgc)++) - firstROMIndex] = pgcValue & 0xff;
							assembledROMBank[((*pgc)++) - firstROMIndex] = (pgcValue >> 8) & 0xff;
							
							return 0x31;
						}
						else {
							assembledROMBank[((*pgc)++) - firstROMIndex] = value & 0xff;
							assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 8) & 0xff;
							assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 16) & 0xff;
							assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
							
							return 0x2d;
						}
					}
					else {
						*requiresMorePasses = TRUE;
						
						return 0x00;
					}
				}
				case asm_TOKEN_REGISTER_24: {
					u8 regNumber = 0x00;
					
					if (lexer->tokenStart[0] == 'S') {
						regNumber = 0x1c;
					}
					else {
						regNumber |= ((lexer->tokenStart[1] - '0') << 2);
					}
					
					switch (asm_getNextToken(lexer, &length)) {
						case asm_TOKEN_PLUS: {
							switch (asm_getNextToken(lexer, &length)) {
								case asm_TOKEN_CONSTANT: {
									s32 constant = asm_decodeConstant(lexer->tokenStart, NULL, TRUE);
									if (!constant && errno) {
										if (pass == 0) {
											char err[err_MAX_ERR_SIZE];
											
											snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding register relative value for RM operand\n", asm_errorString, lexer->lineNum, lexer->colNum);
											strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										}
										
										return -1;
									}
									else if (constant > 0xffffff || constant < -0x8000) {
										if (pass == 0) {
											char err[err_MAX_ERR_SIZE];
											
											snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register relative value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
											strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										}
										
										return -1;
									}
									else if (constant < 0x8000 || constant > 0xff7fff) {
										assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
										
										return 0x01 | regNumber;
									}
									else {
										if (pass == 0) {
											char err[err_MAX_ERR_SIZE];
											
											snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Register relative value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
											strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										}
										
										return -1;
									}
								}
								case asm_TOKEN_REGISTER_8: {
									u8 indexWord = 0x00;
									
									if (lexer->tokenStart[0] == 'M') {
										indexWord |= 0x04;
									}
									
									indexWord |= (lexer->tokenStart[1] - '0');
									
									assembledROMBank[((*pgc)++) - firstROMIndex] = regNumber;
									assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
									
									return 0x39;
								}
								case asm_TOKEN_REGISTER_16: {
									u8 indexWord = 0x40 | ((lexer->tokenStart[1] - '0') << 8);
									
									assembledROMBank[((*pgc)++) - firstROMIndex] = regNumber;
									assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
									
									return 0x39;
								}
								case asm_TOKEN_REGISTER_24: {
									u8 indexWord = 0x80;
									
									if (lexer->tokenStart[0] == 'S') {
										indexWord |= 0x07;
									}
									else {
										indexWord |= ((lexer->tokenStart[1] - '0') << 8);
									}
									
									assembledROMBank[((*pgc)++) - firstROMIndex] = regNumber;
									assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
									
									return 0x39;
								}
								case asm_TOKEN_IDENTIFIER: {
									if (toupper(lexer->tokenStart[2]) == 'S' && toupper(lexer->tokenStart[3]) == 'X' && !asm_IS_ALPHA(lexer->tokenStart[4]) && !asm_IS_DIGIT(lexer->tokenStart[4])) {
										if (asm_IS_REGISTER_8(toupper(lexer->tokenStart[0]), toupper(lexer->tokenStart[1]), ' ')) {
											u8 indexWord = 0x08;
											
											if (lexer->tokenStart[0] == 'M') {
												indexWord |= 0x04;
											}
											
											indexWord |= (lexer->tokenStart[1] - '0');
											
											assembledROMBank[((*pgc)++) - firstROMIndex] = regNumber;
											assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
										
											return 0x39;
										}
										else if (asm_IS_REGISTER_16(toupper(lexer->tokenStart[0]), toupper(lexer->tokenStart[1]), ' ')) {
											u8 indexWord = 0x48 | ((lexer->tokenStart[1] - '0') << 8);
											
											assembledROMBank[((*pgc)++) - firstROMIndex] = regNumber;
											assembledROMBank[((*pgc)++) - firstROMIndex] = indexWord;
											
											return 0x39;
										}
									}
								}
								case asm_TOKEN_LABEL:
								case asm_TOKEN_DIRECTIVE:
								case asm_TOKEN_OPCODE: {
									return 0x20 | regNumber;
								}
								default: {
									if (pass == 0) {
										char err[err_MAX_ERR_SIZE];
										
										snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
										strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									}
									
									return -1;
								}
							}
						}
						case asm_TOKEN_LABEL:
						case asm_TOKEN_DIRECTIVE:
						case asm_TOKEN_OPCODE: {
							return 0x02 | regNumber;
						}
						default: {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							}
							
							return -1;
						}
					}
				}
				case asm_TOKEN_PGC: {
					switch (asm_getNextToken(lexer, &length)) {
						case asm_TOKEN_PLUS: {
							switch (asm_getNextToken(lexer, &length)) {
								case asm_TOKEN_CONSTANT: {
									s32 constant = asm_decodeConstant(lexer->tokenStart, NULL, TRUE);
									if (!constant && errno) {
										if (pass == 0) {
											char err[err_MAX_ERR_SIZE];
											
											snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding PGC relative value for RM operand\n", asm_errorString, lexer->lineNum, lexer->colNum);
											strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										}
										
										return -1;
									}
									else if (constant > 0xffffff || constant < -0x8000) {
										if (pass == 0) {
											char err[err_MAX_ERR_SIZE];
											
											snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: PGC relative value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
											strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										}
										
										return -1;
									}
									else if (constant < 0x8000 || constant > 0xff7fff) {
										assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
										
										return 0x31;
									}
									else {
										assembledROMBank[((*pgc)++) - firstROMIndex] = constant & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 8) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = (constant >> 16) & 0xff;
										assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
										
										return 0x35;
									}
								}
								case asm_TOKEN_IDENTIFIER: {
									asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer->tokenStart, length);
			
									if (symbol == NULL) {
										symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer->tokenStart, length);
										lastSymbol = symbol;
				
										*requiresMorePasses = TRUE;
				
										return 0x00;
									}
									else if (symbol->resolved) {
										s32 value = symbol->value;
										
										if (value > 0xffffff || value < -0x8000) {
											if (pass == 0) {
												char err[err_MAX_ERR_SIZE];
						
												snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Immediate value for RM operand out of range\n", asm_errorString, lexer->lineNum, lexer->colNum);
												strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
											}
											
											return -1;
										}
										if (value < 0x8000 || value > 0xff7fff) {
											assembledROMBank[((*pgc)++) - firstROMIndex] = value & 0xff;
											assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 8) & 0xff;
											
											return 0x31;
										}
										else {
											assembledROMBank[((*pgc)++) - firstROMIndex] = value & 0xff;
											assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 8) & 0xff;
											assembledROMBank[((*pgc)++) - firstROMIndex] = (value >> 16) & 0xff;
											assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
											
											return 0x35;
										}
									}
									else {
										*requiresMorePasses = TRUE;
										
										return 0x00;
									}
								}
								default: {
									if (pass == 0) {
										char err[err_MAX_ERR_SIZE];
								
										snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
										strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									}
							
									return -1;
								}
							}
						}
						case asm_TOKEN_LABEL:
						case asm_TOKEN_DIRECTIVE:
						case asm_TOKEN_OPCODE: {
							assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
							assembledROMBank[((*pgc)++) - firstROMIndex] = 0x00;
							
							return 0x31;
						}
						default: {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							}
							
							return -1;
						}
					}
				}
				case asm_TOKEN_MINUS: {
					switch (asm_getNextToken(lexer, &length)) {
						case asm_TOKEN_REGISTER_24: {
							u8 regNumber = 0x00;
							
							if (lexer->tokenStart[0] == 'S') {
								regNumber = 0x1c;
							}
							else {
								regNumber |= ((lexer->tokenStart[1] - '0') << 2);
							}
							
							return 0x22 | regNumber;
						}
						default: {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							}
							
							return -1;
						}
					}
				}
				default: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
					}
					
					return -1;
				}
			}
		}
		default: {
			if (pass == 0) {
				char err[err_MAX_ERR_SIZE];
				
				snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer->lineNum, lexer->colNum);
				strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
			}
			
			return -1;
		}
	}
}

bool asm_assembleToROMImage(const char *assemblyCode) {
	snprintf(asm_errorString, err_MAX_ERR_SIZE, "");
	
	const char *title = "(No title)";
	const char *developer = "(No developer)";
	
	asm_Lexer lexer;
	
	bool hasError = FALSE;
	
	u32 pgc;
	
	asm_SymbolTableEntry *symbolTable = NULL;
	asm_SymbolTableEntry *lastSymbol = NULL;
	
	u8 romSize = 0x00;
	u32 firstROMIndex = 0xff0000;
	
	u8 *assembledROMBank = asm_reallocateROM(NULL, &romSize, &firstROMIndex);
	
	for (u32 pass = 0; pass < asm_MAX_PASSES; pass++) {
		lexer.current = assemblyCode;
		lexer.lineNum = 1;
		lexer.colNum = 0;
		lexer.hasNewLine = TRUE;
		
		pgc = 0xff0000;
		
		bool requiresMorePasses = FALSE;
		
		while (*lexer.current != '\0') {
			size_t length;
			
			switch (asm_getNextToken(&lexer, &length)) {
				case asm_TOKEN_DIRECTIVE: {
					if (!strncasecmp(lexer.tokenStart, ".ORG", length)) {
						if (asm_getNextToken(&lexer, &length) != asm_TOKEN_CONSTANT) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
							
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .ORG directive needs a literal value\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
						else {
							s32 constant = asm_decodeConstant(lexer.tokenStart, NULL, TRUE);
							if (!constant && errno) {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding literal constant for .ORG directive\n", asm_errorString, lexer.lineNum, lexer.colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									hasError = TRUE;
								}
							}
							else if (constant < 0x010000 || constant > 0xffffff) {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .ORG directive out of range (valid values are $010000-$FFFFFF)\n", asm_errorString, lexer.lineNum, lexer.colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									hasError = TRUE;
								}
							}
							else {
								if (constant < firstROMIndex) {
									firstROMIndex = constant & 0xff0000;
									
									assembledROMBank = asm_reallocateROM(assembledROMBank, &romSize, &firstROMIndex);
								}
								
								pgc = (u32)constant;
							}
						}
						break;
					}
					else if (!strncasecmp(lexer.tokenStart, ".HXH_TITLE", length)) {
						if (asm_getNextToken(&lexer, &length) != asm_TOKEN_STRING) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .HXH_TITLE directive needs a title string\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
						else if (length > 127) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: ROM title is too long (maximum length: 127 bytes)\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
						else {
							memcpy(&assembledROMBank[(romSize << 16) - (0x10000 - 0x9f00)], lexer.tokenStart + sizeof(char), length);
						}
					}
					else if (!strncasecmp(lexer.tokenStart, ".HXH_AUTHOR", length)) {
						if (asm_getNextToken(&lexer, &length) != asm_TOKEN_STRING) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .HXH_AUTHOR directive needs an author string\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
						else if (length > 95) {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: ROM author is too long (maximum length: 95 bytes)\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
						else {
							memcpy(&assembledROMBank[(romSize << 16) - (0x10000 - 0x9f00) + 128], lexer.tokenStart + sizeof(char), length);
						}
					}
					else if (!strncasecmp(lexer.tokenStart, ".DEFINE", length)) {
						if (asm_getNextToken(&lexer, &length) == asm_TOKEN_IDENTIFIER) {
							asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer.tokenStart, length);
							
							if (symbol == NULL) {
								symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer.tokenStart, length);
								lastSymbol = symbol;
							}
							
							if (asm_getNextToken(&lexer, &length) == asm_TOKEN_CONSTANT) {
								s32 constant = asm_decodeConstant(lexer.tokenStart, NULL, TRUE);
							
								if (!constant && errno) {
									if (pass == 0) {
										char err[err_MAX_ERR_SIZE];
									
										snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding literal constant for .DEFINE directive\n", asm_errorString, lexer.lineNum, lexer.colNum);
										strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										hasError = TRUE;
									}
								}
								else {
									symbol->resolved = TRUE;
									symbol->value = constant;
								}
							}
							else {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .DEFINE directive argument must be a literal constant\n", asm_errorString, lexer.lineNum, lexer.colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									hasError = TRUE;
								}
							}
						}
						
					}
					else if (!strncasecmp(lexer.tokenStart, ".DB", length)) {
						asm_Token token = asm_getNextToken(&lexer, &length);
						
						// todo: comma-separated list
						if (token == asm_TOKEN_CONSTANT) {
							s32 constant = asm_decodeConstant(lexer.tokenStart, NULL, TRUE);
							
							if (!constant && errno) {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Error decoding literal constant for .DB directive\n", asm_errorString, lexer.lineNum, lexer.colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									hasError = TRUE;
								}
							}
							else if (constant > 0xff || constant < -0x80) {
								if (pass == 0) {
									char err[err_MAX_ERR_SIZE];
									
									snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Literal constant for .DB directive cannot fit in 1 byte\n", asm_errorString, lexer.lineNum, lexer.colNum);
									strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
									hasError = TRUE;
								}
							}
							else {
								assembledROMBank[(pgc++) - firstROMIndex] = constant & 0xff;
							}
						}
						else if (token == asm_TOKEN_IDENTIFIER) {
							asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer.tokenStart, length);
							
							if (symbol == NULL) {
								symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer.tokenStart, length);
								lastSymbol = symbol;
							}
							
							if (symbol->resolved) {
								if (symbol->value > 0xff || symbol->value < -0x80) {
									if (pass == 0) {
										char err[err_MAX_ERR_SIZE];
										
										snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Literal constant for .DB directive cannot fit in 1 byte\n", asm_errorString, lexer.lineNum, lexer.colNum);
										strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
										hasError = TRUE;
									}
								}
								assembledROMBank[(pgc++) - firstROMIndex] = symbol->value & 0xff;
							}
						}
						else {
							if (pass == 0) {
								char err[err_MAX_ERR_SIZE];
								
								snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: .DB directive needs at least one constant or identifier value\n", asm_errorString, lexer.lineNum, lexer.colNum);
								strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
								hasError = TRUE;
							}
						}
					}
					
					break;
				}
				case asm_TOKEN_OPCODE: {
					// resolve opcodes here
					if (!requiresMorePasses) {
						if (!strncasecmp(lexer.tokenStart, "NOP", length)) {
							assembledROMBank[(pgc++) - firstROMIndex] = 0x00;
							assembledROMBank[(pgc++) - firstROMIndex] = 0x00;
						}
						else if (!strncasecmp(lexer.tokenStart, "HALT", length)) {
							assembledROMBank[(pgc++) - firstROMIndex] = 0x01;
							assembledROMBank[(pgc++) - firstROMIndex] = 0x00;
						}
						else if (!strncasecmp(lexer.tokenStart, "ILG", length)) {
							assembledROMBank[(pgc++) - firstROMIndex] = 0x02;
							assembledROMBank[(pgc++) - firstROMIndex] = 0x00;
						}
					}
					
					break;
				}
				case asm_TOKEN_LABEL: {
					bool isNewSymbol = FALSE;
					
					asm_SymbolTableEntry *symbol = asm_lookupSymbol(&symbolTable, lexer.tokenStart, length);
					
					if (symbol == NULL) {
						symbol = asm_addSymbol(&symbolTable, lastSymbol, lexer.tokenStart, length);
						lastSymbol = symbol;
						
						isNewSymbol = TRUE;
					}
					
					if (symbol != NULL && symbol->resolved) {
						if (pass == 0) {
							char err[err_MAX_ERR_SIZE];
							
							snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Label has already been defined\n", asm_errorString, lexer.lineNum, lexer.colNum);
							strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
							hasError = TRUE;
						}
					}
					else if (!requiresMorePasses) {
						if (!isNewSymbol) {
							requiresMorePasses = TRUE;
						}
						
						symbol->resolved = TRUE;
						symbol->value = (s32)pgc;
					}
					
					break;
				}
				case asm_TOKEN_ERROR:
				default: {
					if (pass == 0) {
						char err[err_MAX_ERR_SIZE];
						
						snprintf(err, err_MAX_ERR_SIZE, "%sLine %d, column %d: Syntax error\n", asm_errorString, lexer.lineNum, lexer.colNum);
						strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
						hasError = TRUE;
						break;
					}
				}
			}
		}
		
		if (hasError) {
			return !hasError;
		}
		
		bool allResolved = !requiresMorePasses;
		if (symbolTable != NULL) {
			for (asm_SymbolTableEntry *sym = symbolTable; sym->next != NULL; sym = sym->next) {
				if (!sym->resolved) {
					allResolved = FALSE;
					break;
				}
			}
		}
		
		if (allResolved) {
			return !hasError;
		}
	}
	
	char err[err_MAX_ERR_SIZE];
	snprintf(err, err_MAX_ERR_SIZE, "%sSymbol resolution failed (tried %d times)\n", asm_errorString, asm_MAX_PASSES);
	strncpy(asm_errorString, err, err_MAX_ERR_SIZE);
	
	return FALSE;
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
