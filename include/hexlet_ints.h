/* Short names for n-bit types in Hexlet */

#ifndef HEXLET_INTS_H
#define HEXLET_INTS_H

#if __STDC_VERSION__ < 199901L

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

#else

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

#endif

#endif