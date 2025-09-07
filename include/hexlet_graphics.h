/* Header file for Hexlet's graphics engine */

#ifndef HEXLET_GFX_H
#define HEXLET_GFX_H

#include <hexlet_ints.h>
#include <hexlet_bools.h>

typedef u8 gfx_SevenSegmentIndexMask;
#define gfx_SEVEN_SEGMENT_INDEX_1	0x01
#define gfx_SEVEN_SEGMENT_INDEX_2	0x02
#define gfx_SEVEN_SEGMENT_INDEX_3	0x04
#define gfx_SEVEN_SEGMENT_INDEX_4	0x08
#define gfx_SEVEN_SEGMENT_INDEX_5	0x10
#define gfx_SEVEN_SEGMENT_INDEX_6	0x20
#define gfx_SEVEN_SEGMENT_INDEX_7	0x40
#define gfx_SEVEN_SEGMENT_INDEX_8	0x80

typedef u8 gfx_SevenSegmentMask;
#define gfx_SEVEN_SEGMENT_NORTH		0x01
#define gfx_SEVEN_SEGMENT_NORTHEAST	0x02
#define gfx_SEVEN_SEGMENT_SOUTHEAST	0x04
#define gfx_SEVEN_SEGMENT_SOUTH		0x08
#define gfx_SEVEN_SEGMENT_SOUTHWEST	0x10
#define gfx_SEVEN_SEGMENT_NORTHWEST	0x20
#define gfx_SEVEN_SEGMENT_CENTER	0x40

typedef struct {
	u8 width;
	u8 height;
	
	u8 *data;	/* length of data = (width * height) / 2 */
} gfx_Bitmap;

typedef struct {
	u8 xPos;
	u8 yPos;
	u8 width;
	u8 height;
	
	bool hasPixels;
	union {
		u8 color;
		gfx_Bitmap bitmap;
	} data;
} gfx_Rect;

/*
*  Get the string representing the last error from the graphics engine.
*/
char *gfx_getError(void);

#endif