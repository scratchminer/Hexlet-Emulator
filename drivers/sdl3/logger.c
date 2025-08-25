/* Source file for Hexlet's logger */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexlet_ints.h>

#include "logger.h"

static u8 log_tableColumns;
static u8 log_colWidths[log_MAX_TABLE_COLS];
static u8 log_currentRow;
static char *log_table[log_MAX_TABLE_ROWS][log_MAX_TABLE_COLS];

void log_printInfo(char *msg) {
	printf("%s\n", msg);
}

void log_printError(char *msg) {
	fprintf(stderr, "hexlet: error: %s\n", msg);
}

void log_startTable(u8 columns) {
	log_tableColumns = columns;
	memset(log_colWidths, 0, log_MAX_TABLE_COLS);
	log_currentRow = 0;
}

void log_printTable(char *msg, ...) {
	va_list columns;
	va_start(columns, msg);
	
	for (u8 col = 0; col < log_tableColumns; col++) {
		char *currentCell;
		
		if (col) currentCell = va_arg(columns, char *);
		else currentCell = msg;
		
		if (log_colWidths[col] < (strlen(currentCell) + 2)) log_colWidths[col] = strlen(currentCell) + 2;
		log_table[log_currentRow][col] = currentCell;
	}
	
	log_currentRow++;
	va_end(columns);
}

void log_printTableRow(char *row) {
	/* we need some way to mark a full row, so use NULL as the first column and the repeat string as the second */
	log_table[log_currentRow][0] = NULL;
	log_table[log_currentRow][1] = row;
	log_currentRow++;
}

void log_endTable(void) {
	for (u8 row = 0; row < log_currentRow; row++) {
		if (log_table[row][0] == NULL) {
			/* get the table's total width */
			u16 sum = 0;
			for (u8 col = 0; col < log_tableColumns; col++) {
				sum += log_colWidths[col];
			}
			
			/* print until we get there */
			for (u16 chNum = 0; chNum < sum - 2;) {
				chNum += printf("%s", log_table[row][1]);
			}
		}
		else {
			for (u8 col = 0; col < log_tableColumns; col++) {
				u8 length = printf("%s", log_table[row][col]);
				if (col != log_tableColumns - 1) {
					for (u8 i = length; i < log_colWidths[col]; i++) printf(" ");
				}
			}
		}
		printf("\n");
	}	
}