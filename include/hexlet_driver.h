/* Driver header file for Hexlet */

#ifndef HEXLET_DRIVER_H
#define HEXLET_DRIVER_H

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_graphics.h>

/* All methods except drv_getError should return FALSE on failure or TRUE on success. */

/*
*  Get the last error message from the driver (meant for logging).
*/
char *drv_getError(void);

/*
*  Called when the emulator is started (meant for driver initialization).
*/
boolean drv_initDriver(u8 displayScale);

/*
*  Called repeatedly in the main loop (meant to be called every frame).
*  Don't tick any of the console's components here; that's already done in the main loop.
*/
boolean drv_nextFrame(void);

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
*  Note: For ease of use, the RGB values are in standard RGB, not the Hexheld's funky backlight color space.
*/
boolean drv_setBacklight(u8 r, u8 g, u8 b);

/*
*  Set the seven-segment displays specified by indexMask to the value specified by segmentMask.
*/
boolean drv_setSevenSegment(gfx_SevenSegmentIndexMask indexMask, gfx_SevenSegmentMask segmentMask);

#endif