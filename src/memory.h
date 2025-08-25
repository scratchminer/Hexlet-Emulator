/* Internal header file for Hexlet's memory bus emulator */

#ifndef HEXLET_MEMORY_H_INTERNAL
#define HEXLET_MEMORY_H_INTERNAL

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_memory.h>

typedef struct {
	bool monitored;
	u32 address;
	u16 data;
} mem_Bus;

#define mem_WRAM_SIZE 32768
#define mem_VRAM_SIZE 32768
#define mem_TMRAM_SIZE 4096
#define mem_HRAM_SIZE 3072

typedef struct {
	u16 wram[mem_WRAM_SIZE / 2];
	u8 vram[mem_VRAM_SIZE];
	
	/* could be set on a write to the boot ROM? */
	bool bootRomLock;
	
	u32 romLength;
	u8 *rom;
	
	u8 tmram[mem_TMRAM_SIZE];
	u16 hram[mem_HRAM_SIZE / 2];
	
	/* used for debugging */
	mem_Bus cpuBus;
	mem_Bus ppuBus;
	mem_Bus hexridgeBus;
} mem_Memory;

mem_Memory *mem_getCurrentMemory(void);

#endif