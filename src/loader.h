/* Internal header file for Hexlet's file loader */

#ifndef HEXLET_LDR_H_INTERNAL
#define HEXLET_LDR_H_INTERNAL

#include <hexlet_ints.h>
#include <hexlet_loader.h>
#include <hexlet_version.h>
#include "pilot.h"

/* system byte order stuff */
const int endianCheck = 1;
#define ldr_BIG_ENDIAN() ((*(char *)&endianCheck) == 0)
#define ldr_ENDIAN_REVERSE_16(s) (((s & 0xff00) >> 8) | ((s & 0x00ff) << 8))
#define ldr_ENDIAN_REVERSE_32(s) (((s & 0xff000000) >> 24) | ((s & 0x00ff0000) >> 8) | ((s & 0x0000ff00) << 8) | ((s & 0x000000ff) << 24))
#define ldr_LITTLE_ENDIAN_16(s) (ldr_BIG_ENDIAN() ? ldr_ENDIAN_REVERSE_16((u16)s) : (u16)s)
#define ldr_LITTLE_ENDIAN_32(s) (ldr_BIG_ENDIAN() ? ldr_ENDIAN_REVERSE_32((u32)s) : (u32)s)

#define ldr_ROM_IMAGE_MAGIC "HEXHELD SOFTWARE"

typedef u8 ldr_ChipType;
#define ldr_CHIP_TYPE_NONE 0x00
#define ldr_CHIP_TYPE_NVRAM 0x01

/* more stuff would go here... */

/* This flag is set if the chip's contents are stored in the ROM image, but it shouldn't be set in an actual cartridge ROM */
#define ldr_CHIP_TYPE_STORED 0x80

typedef struct {
	u8 data[256];
	
	char title[128];
	char author[96];
	
	/* Size is in 64-KiB units (i.e. cannot be more than 0xdf) */
	u8 romSize;
	
	ldr_ChipType cs1;
	ldr_ChipType cs2;
	
	u8 minHiveCraftVersion;
	u16 softwareRevision;
	
	u16 copyrightStartYear;
	u16 copyrightEndYear;
	
	u16 crc;
} ldr_ROMHeader;

typedef struct {
	u32 length;
	u8 *data;
} ldr_StateFileChunk;

typedef struct {
	ldr_ROMHeader *header;
	ldr_StateFileChunk *rom;
	ldr_StateFileChunk *cs1;
	ldr_StateFileChunk *cs2;
} ldr_ROMImage;

#define ldr_STATE_FILE_MAGIC "Hexlet\x1b"

typedef struct {
	u8 data[48];
	
	ver_Version *version;
	ldr_StateFileFlags dataStored;
	
	u8 hiveCraftVersion;
	
	ldr_StateFileChunk *wram;
	ldr_StateFileChunk *vram;
	ldr_StateFileChunk *tmram;
	
	cpu_Pilot *cpuState;
	
	ldr_StateFileChunk *cs1;
	ldr_StateFileChunk *cs2;
	ldr_StateFileChunk *oam;
} ldr_StateFile;

#endif