/* Header file for Hexlet's file loader */

#ifndef HEXLET_LDR_H
#define HEXLET_LDR_H

#include <hexlet_ints.h>
#include <hexlet_bools.h>

typedef u8 ldr_StateFileFlags;
#define ldr_STATE_FILE_FLAG_STORE_WRAM	0x80
#define ldr_STATE_FILE_FLAG_STORE_VRAM	0x40
#define ldr_STATE_FILE_FLAG_STORE_HRAM	0x20
#define ldr_STATE_FILE_FLAG_STORE_CPU	0x10
#define ldr_STATE_FILE_FLAG_STORE_CS1	0x08
#define ldr_STATE_FILE_FLAG_STORE_CS2	0x04
#define ldr_STATE_FILE_FLAG_STORE_OAM	0x02

/*
*  Get the error message representing the last error from the loader.
*/
char *ldr_getError(void);

/*
*  Load a ROM image (standard extension .hxh) from the specified data buffer, reading at most length bytes.
*  Return FALSE on failure or TRUE on success.
*  If length is 0, this will read from the buffer until a valid ROM image has been constructed.
*/
boolean ldr_loadROMImage(void *data, u32 length);

/*
*  Get the size in bytes of the loaded ROM image as a file. Return 0 on failure.
*/
u32 ldr_getROMImageSize(void);

/*
*  Save the loaded ROM image to the specified data buffer, writing at most length bytes. Return FALSE on failure or TRUE on success.
*/
boolean ldr_saveROMImage(u8 *data, u32 length);

/*
*  Load a state (standard extension .hxl) from the specified data buffer, reading at most length bytes.
*  Return FALSE on failure or TRUE on success.
*  If length is 0, this will read from the buffer until a valid state has been constructed.
*/
boolean ldr_loadState(u8 *data, u32 length);

/*
*  Get the size in bytes of the specified parts of the state as a file. Return 0 on failure.
*/
u32 ldr_getStateSize(ldr_StateFileFlags getWhat);

/*
*  Save the specified parts of the state to the specified data buffer, writing at most length bytes. 
*  Return FALSE on failure or TRUE on success.
*/
boolean ldr_saveState(u8 *data, u32 length, ldr_StateFileFlags saveWhat);

#endif