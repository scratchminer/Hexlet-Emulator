/* Driver header file for Hexlet */

#ifndef HEXLET_DRIVER_H
#define HEXLET_DRIVER_H

#include <stdlib.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_graphics.h>

/* All methods should return FALSE on failure or TRUE on success. */

/*
*  Plot a hexagonal pixel at the given coordinates with the given intensity.
*/
bool drv_plotPix(u8 x, u8 y, u8 intensity);

/*
*  Copy the given gfx_Rect to the screen.
*  This should be at least as fast as calling drv_plotPix for each pixel copied.
*/
bool drv_copyRect(gfx_Rect *rect);

/*
*  Set the backlight color to be (r, g, b).
*  Note: For ease of use, the RGB values are in standard RGB, not in the Hexheld's funky backlight color space.
*/
bool drv_setBacklight(u8 r, u8 g, u8 b);

/*
*  Set the seven-segment displays specified by indexMask to the value specified by segmentMask.
*/
bool drv_setSevenSegment(gfx_SevenSegmentIndexMask indexMask, gfx_SevenSegmentMask segmentMask);

/*
*  Macros so gfx_<functionName>() can be used for multi-file drivers
*/
#define gfx_plotPix drv_plotPix
#define gfx_copyRect drv_copyRect
#define gfx_setBacklight drv_setBacklight
#define gfx_setSevenSegment drv_setSevenSegment

/*
*  Memory allocation/reallocation/freeing function, used sparingly.
*  When oldPtr is NULL, allocate a block of memory with size newSize and return a pointer to the new block.
*  When newSize is 0, free the block of memory given by oldPtr with size oldSize and return NULL.
*  Otherwise, reallocate the block of memory given by oldPtr from size oldSize to size newSize and return a pointer to the new block.
*/
void *drv_reallocate(void *oldPtr, size_t oldSize, size_t newSize);

#endif