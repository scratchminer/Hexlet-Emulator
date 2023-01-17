/* Internal header file for Hexlet's Pilot CPU emulator */

#ifndef HEXLET_CPU_H_INTERNAL
#define HEXLET_CPU_H_INTERNAL

#include <hexlet_ints.h>

#include "memory.h"

typedef u16 cpu_StatusRegMask;
#define cpu_IRQ_LEVEL_MASK	0x0700
#define cpu_FLAG_SIGN		0x0080
#define cpu_FLAG_ZERO		0x0040
#define cpu_FLAG_OVERFLOW	0x0004
#define cpu_FLAG_DECIMAL	0x0002
#define cpu_FLAG_EXTEND		0x0001

typedef struct {
	u32 regs[8];
	u16 statusReg;
	u32 programCounter;
	
	/* Spweesh, if you're reading this, is this still supposed to be here? */
	u16 prefetchQueue[2];
} cpu_Pilot;

/*
*  Perform a single CPU cycle with the specified memory bus.
*  Note: This method does not check whether the CPU should actually be ticked (e.g. if a DMA is in progress, it shouldn't be).
*/
void cpu_tickPilot(cpu_Pilot pilot, mem_Memory memory);

#endif