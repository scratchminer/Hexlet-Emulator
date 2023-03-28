/* Source file for Hexlet's ROM image and state file loader */

#include <stdio.h>
#include <string.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_version.h>

#include "errors.h"
#include "loader.h"
#include "memory.h"

char ldr_errorString[err_MAX_ERR_SIZE];
static ldr_ROMImage ldr_currentROM;
static ldr_StateFileChunk cs2Chunk;
static ldr_StateFileChunk cs1Chunk;

char *ldr_getError(void) {
	return ldr_errorString;
}

boolean ldr_loadROMImage(void *data, u32 length) {
	snprintf(ldr_errorString, err_MAX_ERR_SIZE, "");
	
	if(length < 256) {
		snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: length must be at least 256 bytes");
		return FALSE;
	}
	
	ldr_ROMHeader header;
	memset(&header, 0, sizeof(header));
	
	memcpy(header.data, data, 256);
	boolean noErrors = TRUE;
	
	char *ptrStr = data;
	snprintf(header.title, 128, "%s", ptrStr);
	snprintf(header.author, 96, "%s", (ptrStr += 128));
	
	if(!strncmp(ptrStr += 96, ldr_ROM_IMAGE_MAGIC, 16)){
		snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Incorrect magic sequence");
		noErrors = FALSE;
	}
	
	u8 *ptrByte = data + 0xf0;
	header.romSize = *ptrByte;
	
	header.cs1 = *(++ptrByte);
	header.cs2 = *(++ptrByte);
	header.minHiveCraftVersion = *ptrByte;
	if(header.minHiveCraftVersion > ver_MAX_HIVECRAFT_VERSION() && noErrors) {
		snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Emulator is too old (needs SoC version $%.02X)", header.minHiveCraftVersion);
		noErrors = FALSE;
	}
	
	u16 *ptrWord = data + 0xf8;
	
	header.softwareRevision = ldr_LITTLE_ENDIAN_16(*ptrWord);
	ptrWord += 2;
	header.copyrightStartYear = ldr_LITTLE_ENDIAN_16(*ptrWord);
	ptrWord += 2;
	header.copyrightEndYear = ldr_LITTLE_ENDIAN_16(*ptrWord);
	ptrWord += 2;
	header.crc = ldr_LITTLE_ENDIAN_16(*ptrWord);
	
	ldr_currentROM.header = &header;
	
	u32 offset = 256;
	ptrByte = data;
	
	if(offset < length) {
		ldr_StateFileChunk romChunk;
		romChunk.length = ldr_LITTLE_ENDIAN_32(*(ptrByte + offset)) & 0xffffff;
	
		if((offset += 2) < length && (length - offset) > romChunk.length) {
			romChunk.data = ptrByte + offset;
			offset += romChunk.length;
			ldr_currentROM.rom = &romChunk;
		}
		else {
			snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Incomplete ROM chunk detected");
			return FALSE;
		}
	}
	else {
		snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Incomplete ROM chunk detected");
		return FALSE;
	}
	
	if(offset < length) {
		if((header.cs1 & ldr_CHIP_TYPE_STORED)) {
			cs1Chunk.length = ldr_LITTLE_ENDIAN_32(*(ptrByte + offset)) & 0xffffff;
	
			if((offset += 2) < length && (length - offset) > cs1Chunk.length) {
				cs1Chunk.data = ptrByte + offset;
				offset += cs1Chunk.length;
				ldr_currentROM.cs1 = &cs1Chunk;
			}
			else {
				snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Incomplete CS1 chunk detected");
				return FALSE;
			}
		}
	}
	else {
		return TRUE;
	}
	
	if(offset < length) {
		if((header.cs1 & ldr_CHIP_TYPE_STORED)) {
			cs2Chunk.length = ldr_LITTLE_ENDIAN_32(*(ptrByte + offset)) & 0xffffff;
	
			if((offset += 2) < length && (length - offset) > cs2Chunk.length) {
				cs2Chunk.data = ptrByte + offset;
				offset += cs2Chunk.length;
				ldr_currentROM.cs1 = &cs2Chunk;
			}
			else {
				snprintf(ldr_errorString, err_MAX_ERR_SIZE, "Error loading ROM: Incomplete CS2 chunk detected");
				return FALSE;
			}
		}
	}
	
	return noErrors;
}