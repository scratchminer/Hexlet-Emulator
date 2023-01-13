/* Internal header file for Hexlet's Pilot CPU emulator */

#ifndef HEXLET_CPU_H_INTERNAL
#define HEXLET_CPU_H_INTERNAL

#include <hexlet_ints.h>

#include "memory.h"

typedef u16 cpu_StatusRegMask;
#define cpu_FLAG_IRQ7_EN	0x8000
#define cpu_FLAG_IRQ6_EN	0x4000
#define cpu_FLAG_IRQ5_EN	0x2000
#define cpu_FLAG_IRQ4_EN	0x1000
#define cpu_FLAG_IRQ3_EN	0x0800
#define cpu_FLAG_IRQ2_EN	0x0400
#define cpu_FLAG_IRQ1_EN	0x0200
#define cpu_FLAG_IRQ0_EN	0x0100

#define cpu_FLAG_SIGN		0x0020
#define cpu_FLAG_ZERO		0x0010
#define cpu_FLAG_MASTER_IRQ_EN	0x0008
#define cpu_FLAG_HALF_CARRY	0x0004
#define cpu_FLAG_OVERFLOW	0x0002
#define cpu_FLAG_CARRY		0x0001

typedef struct {
	u32 regs[7];
	u32 stackPointer;
	u16 statusReg;
} cpu_Registers;

typedef struct {
	cpu_Registers *regs;
	u32 programCounter;
	
	u16 prefetchQueue[2];
} cpu_Pilot;

/*
*  Perform a single CPU cycle with the specified memory bus.
*  Note: This method does not check whether the CPU should actually be ticked (e.g. if a DMA is in progress, it shouldn't be).
*/
void cpu_tickPilot(cpu_Pilot pilot, mem_Memory memory);

#endif