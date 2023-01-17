/* Driver header file for Hexlet */

#ifndef HEXLET_DRIVER_H
#define HEXLET_DRIVER_H

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_graphics.h>

/* All methods should return FALSE on failure or TRUE on success. */

/*
*  Plot a hexagonal pixel at the given coordinates with the given intensity.
*/
boolean drv_plotPix(u8 x, u8 y, u8 intensity);

/*
*  Copy the given gfx_Rect to the screen.
*  This should be faster than calling drv_plotPix for each pixel copied.
*/
boolean drv_copyRect(gfx_Rect *rect);

/*
*  Set the backlight color to be (r, g, b).
*  Note: For ease of use, the RGB values are in standard RGB, not in the Hexheld's funky backlight color space.
*/
boolean drv_setBacklight(u8 r, u8 g, u8 b);

/*
*  Set the seven-segment displays specified by indexMask to the value specified by segmentMask.
*/
boolean drv_setSevenSegment(gfx_SevenSegmentIndexMask indexMask, gfx_SevenSegmentMask segmentMask);

/*
*  Main method.
*/
int main(int argc, char **argv);

/*
*  Macros so you can use gfx_function() instead of drv_function()
*/
#define gfx_plotPix drv_plotPix
#define gfx_copyRect drv_copyRect
#define gfx_setBacklight drv_setBacklight
#define gfx_setSevenSegment drv_setSevenSegment

#endif