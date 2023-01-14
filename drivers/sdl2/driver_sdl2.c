/* Sample driver for Hexlet using SDL2 */

#include <stdio.h>

#include <SDL.h>

#include <hexlet_driver.h>
#include <hexlet_errs.h>
#include <hexlet_loader.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window *sdlWindow;
SDL_Surface *sdlSurf;

char lastError[err_MAX_ERR_SIZE];

char *drv_getError(void) {
	return lastError;
}

boolean drv_initDriver(u8 displayScale) {
	/* SDL initialization */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		snprintf(lastError, sizeof(lastError), "Failed to initialize SDL:\n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	sdlWindow = SDL_CreateWindow("Hexlet",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		displayScale * SCREEN_WIDTH, displayScale * SCREEN_HEIGHT, 
		0);
	if(!sdlWindow) {
		snprintf(lastError, sizeof(lastError), "Failed to create an SDL window:\n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	sdlSurf = SDL_GetWindowSurface(sdlWindow);
	if(!sdlSurf) {
		snprintf(lastError, sizeof(lastError), "Failed to create an SDL surface from a window: \n\t\t%s", SDL_GetError());
		return FALSE;
	}
	
	return TRUE;
}

boolean drv_nextFrame(void) {
	
}

boolean drv_plotPix(u8 x, u8 y, u8 intensity) {
	
}

boolean drv_copyRect(gfx_Rect *rect) {
	
}

boolean drv_setBacklight(u8 r, u8 g, u8 b) {
	
}

boolean drv_setSevenSegment(gfx_SevenSegmentIndexMask indexMask, gfx_SevenSegmentMask segmentMask) {
	
}