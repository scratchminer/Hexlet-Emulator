/* Internal header file for Hexlet's file loader */

#ifndef HEXLET_LDR_H_INTERNAL
#define HEXLET_LDR_H_INTERNAL

#include <hexlet_ints.h>
#include <hexlet_loader.h>
#include <hexlet_version.h>
#include "pilot.h"

#define ldr_ROM_IMAGE_MAGIC		"HEXHELD SOFTWARE"

typedef u8 ldr_ChipType;
#define ldr_CHIP_TYPE_ROM		0x00
#define ldr_CHIP_TYPE_SRAM		0x01
#define ldr_CHIP_TYPE_NVRAM		0x02

/* more stuff would go here... */

#define ldr_CHIP_TYPE_NONE		0x7f
#define ldr_CHIP_TYPE_COUNT		ldr_CHIP_TYPE_NVRAM

/* This flag is set if the chip's contents are stored in the ROM image */
#define ldr_CHIP_TYPE_STORED	0x80

typedef struct {
	u8 data[256];
	
	char title[128];
	char author[64];
	
	/* Size is in 64-KiB segments (i.e. cannot be more than 0xfe) */
	u8 romSize;
	
	ldr_ChipType cs1;
	ldr_ChipType cs2;
	
	u8 startingBusCtrl;
	
	u8 minHiveCraftVersion;
	
	u16 copyrightStartYear;
	u16 copyrightEndYear;
	
	u8 sha256[32];
} ldr_ROMHeader;

#define ldr_CACHE_FILE_MAGIC	"Hexlet\x1b"

typedef struct {
	u32 length;
	u8 *data;
} ldr_CacheFileChunk;

typedef struct {
	u8 data[48];
	
	ver_Version *version;
	ldr_CacheFileFlags dataStored;
	
	u8 hiveCraftVersion;
	
	ldr_CacheFileChunk *wram;
	ldr_CacheFileChunk *vram;
	ldr_CacheFileChunk *tmram;
	ldr_CacheFileChunk *ioStates;
	cpu_Pilot *cpuState;
	ldr_CacheFileChunk *cs1;
	ldr_CacheFileChunk *cs2;
	ldr_CacheFileChunk *oam;
} ldr_CacheFile;

#endif