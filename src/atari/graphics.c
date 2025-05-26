// (C) 2025 Brad Colbert

#include "graphics.h"
#include "playfield.h"

#include <atari.h>

#include <stdint.h>

// Defines
#define GFX_9 0x40

// Globals

// Imports
extern tile_struct row_zero[]; // Defined in playfield_and_data.c
extern tile_struct row_one[];  // Defined in playfield_and_data.c
extern tile_struct row_two[];  // Defined in playfield_and_data.c
extern tile_struct row_three[]; // Defined in playfield_and_data.c

extern void display_list_anticF;

void init_graphics()
{
    // Initialize graphics mode
    OS.sdlst = &display_list_anticF;   // Show the logo
    OS.gprior |= GFX_9;                // Enable GTIA (graphics 9)

}

void render_frame()
{
    // Render the current frame
}

void shutdown_graphics()
{
    // Restore the starting graphics mode
}
