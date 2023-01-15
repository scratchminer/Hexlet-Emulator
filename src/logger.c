/* Source file for Hexlet's logger */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexlet_ints.h>

#include "logger.h"

u8 log_tableColumns;
u8 log_colWidths[log_MAX_TABLE_COLS];
u8 log_currentRow;
char *log_table[log_MAX_TABLE_ROWS][log_MAX_TABLE_COLS];

void log_printInfo(char *msg) {
	printf("%s\n", msg);
}

void log_printError(char *msg) {
	fprintf(stderr, "hexlet: error: %s\n", msg);
}

void log_startTable(u8 columns) {
	if(columns > 1) log_tableColumns = columns;
}

void log_printTable(char *msg, ...) {
	va_list columns;
	va_start(columns, msg);
	
	for(u8 col = 0; col < log_tableColumns; ++col) {
		char *currentCell = va_arg(columns, char *);
		
		if(log_colWidths[col] < (strlen(currentCell) + 2)) log_colWidths[col] = strlen(currentCell) + 2;
		log_table[log_currentRow][col] = currentCell;
	}
	
	++log_currentRow;
	va_end(columns);
}

void log_printTableRow(char *row) {
	/* we need some way to mark a full row, so use NULL as the first column and the repeat string as the second */
	log_table[log_currentRow][0] = NULL;
	log_table[log_currentRow][0] = row;
}

void log_endTable(void) {
	for(u8 row = 0; row < log_currentRow; ++row) {
		if(log_table[row][0] == NULL) {
			for(u8 col = 0; col < log_tableColumns;) {
				col += printf("%s", log_table[row][1]);
			}
		}
		else {
			for(u8 col = 0; col < log_tableColumns;) {
				u8 length = printf("%s", log_table[row][col]);
				for(u8 i = length; i < log_colWidths[col]; ++i) printf(" ");
			}
		}
		printf("\n");
	}
	
	log_tableColumns = 0;
	memset(log_colWidths, 0, log_MAX_TABLE_COLS);
	log_currentRow = 0;	
}