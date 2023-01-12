/* Internal header file for Hexlet's Pilot CPU emulator */

#ifndef HEXLET_CPU_H_INTERNAL
#define HEXLET_CPU_H_INTERNAL

#include <hexlet_ints.h>

typedef u8 cpu_FlagMask;
#define cpu_FLAG_SIGN		0x80
#define cpu_FLAG_ZERO		0x40
#define cpu_FLAG_INTERRUPT	0x20
#define cpu_FLAG_HALF_CARRY	0x10
#define cpu_FLAG_ADJUST		0x08
#define cpu_FLAG_OVERFLOW	0x04
#define cpu_FLAG_DATA_MODE	0x02
#define cpu_FLAG_CARRY		0x01

typedef struct {
	u8 a;
	u8 b;
	u8 h;
	u8 l;
	u8 i;
	u8 x;
	u8 d;
	u8 s;
	u8 c;
	u8 f;
} cpu_HalfRegisters;

typedef struct {
	cpu_HalfRegisters *visibleRegs;
	cpu_HalfRegisters *shadowRegs;
	
	u8 k;
	u8 e;
} cpu_Registers;

typedef struct {
	cpu_Registers *regs;
	u16 stackPointer;
	u16 programCounter;
	
	u16 prefetchQueue[2];
} cpu_Pilot;

/*
*  Perform a single CPU cycle with the memory and I/O buses specified.
*  Note: This method does not check whether the CPU should actually be ticked (e.g. if a DMA is in progress, it shouldn't be)
*/
void cpu_tickPilot(cpu_Pilot pilot, mem_Memory memory, iob_InputOutput io);

#endif