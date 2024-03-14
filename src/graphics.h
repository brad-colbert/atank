#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "types.h"

#define GFX_ANTIC4

extern byte* DL;

// Setup the graphics mode
void init_graphics(void);

// Restore graphics mode
void close_graphics(void);

#endif __GRAPHICS_H__