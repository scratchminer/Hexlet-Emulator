/* Header file for Hexlet's memory bus emulator (could be used for debugging) */

#ifndef HEXLET_MEMORY_H_INTERNAL
#define HEXLET_MEMORY_H_INTERNAL

#include <hexlet_ints.h>
#include <hexlet_bools.h>

typedef u8 mem_BusType;
#define mem_BUS_TYPE_CPU		0x01
#define mem_BUS_TYPE_PPU		0x02
#define mem_BUS_TYPE_HEXRIDGE	0x04

/* By default, Hexlet monitors no buses to save host CPU time. */

/*
*  Set which bus or buses Hexlet should monitor (more than one can be passed in).
*/
void mem_monitorBuses(mem_BusType buses);

/*
*  Return the content of the address lines of a bus being monitored.
*  If multiple buses are provided, their address lines are OR'd together to provide the result.
*/
u32 mem_getAddress(mem_BusType buses);

/*
*  Return the content of the data lines of a bus being monitored.
*  If multiple buses are provided, their data lines are OR'd together to provide the result.
*/
u16 mem_getData(mem_BusType buses);

#endif