/* Internal header file for Hexlet's logger */

#ifndef HEXLET_LOG_H_INTERNAL
#define HEXLET_LOG_H_INTERNAL

#include <hexlet_ints.h>

/* 
*  Maximum rows and columns a table is allowed to have
*/
#define log_MAX_TABLE_ROWS 256
#define log_MAX_TABLE_COLS 256

/*
*  Minimum number of spaces between table cells
*/
#define log_TABLE_COL_PADDING 2

/*
*  Print an informative message to the console with a newline appended.
*/
void log_printInfo(char *msg);

/*
*  Print an error message to stderr with a newline appended.
*/
void log_printError(char *msg);

/*
*  Start processing a table with a specified number of columns greater than 1.
*/
void log_startTable(u8 columns);

/*
*  Add a row to the table with the specified cell contents.
*  Uses varargs of type 'char *' for the columns.
*/
void log_printTable(char *msg, ...);

/*
*  Add a row of repeats of the specified string for the width of the table.
*/
void log_printTableRow(char *row);

/*
*  Flush the table buffer and print the table previously created with log_startTable().
*/
void log_endTable(void);

#endif