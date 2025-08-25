/* Graphics source file for Hexlet's sample SDL3 driver */

#include <stdio.h>

#include <SDL3/SDL.h>

#include <hexlet_ints.h>
#include <hexlet_bools.h>
#include <hexlet_graphics.h>
#include <hexlet_driver.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static SDL_Window *sdlWindow;
static SDL_Surface *sdlSurf;

bool gfx_initDriver(u8 displayScale) {
	char *lastError;
	
	/* SDL initialization */
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		snprintf(lastError, sizeof(lastError), "Failed to initialize SDL:\n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	sdlWindow = SDL_CreateWindow("Hexlet", displayScale * SCREEN_WIDTH, displayScale * SCREEN_HEIGHT, 0);
	if (!sdlWindow) {
		snprintf(lastError, sizeof(lastError), "Failed to create an SDL window:\n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	sdlSurf = SDL_GetWindowSurface(sdlWindow);
	if (!sdlSurf) {
		snprintf(lastError, sizeof(lastError), "Failed to create an SDL surface from a window: \n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	return TRUE;
}

bool gfx_nextFrame(void) {
	return FALSE;
}

bool gfx_plotPix(u8 x, u8 y, u8 intensity) {
	return FALSE;
}

bool gfx_copyRect(gfx_Rect *rect) {
	return FALSE;
}

bool gfx_setBacklight(u8 r, u8 g, u8 b) {
	return FALSE;
}

bool gfx_setSevenSegment(gfx_SevenSegmentIndexMask indexMask, gfx_SevenSegmentMask segmentMask) {
	return FALSE;
}