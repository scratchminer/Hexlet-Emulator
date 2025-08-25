/* Header file for Hexlet's emulator */

#ifndef HEXLET_EMULATE_H
#define HEXLET_EMULATE_H

/*
*  Get the string representing the last error from the emulator.
*/
char *emu_getError(void);

/*
*  Decode a constant value formatted in the correct way: $00 -> hex, %00000000 -> binary, 0 -> decimal
*  Return 0 on failure.
*  A check for errors is needed when calling this, since 0 is also a valid value.
*/
s32 emu_decodeConstant(char *number);

/*
*  Step the emulator through one frame and update the screen buffer.
*  Return FALSE on failure or TRUE on success.
*  Note: You don't have to pass the buffer of the previous frame; it can be any gfx_Bitmap. This could be useful for a debugger.
*/
//bool emu_tick(gfx_Bitmap *screen);

#endif