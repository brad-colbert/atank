// (C) 2025 Brad Colbert

#include "graphics.h"
#include "playfield.h"
#include "player.h"

#include <atari.h>
#include <conio.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Temp
void load_map(void);

// Defines
#define GFX_9 0x40

// Imports
extern void display_list_anticF;
extern void display_list_antic4;
extern bool scroll_flag;                    // Defined in playfield_scroll.s
extern graphics_state saved_graphics_state; // Defined in playfield_and_data.c
extern uint8_t font[];                      // Defined in atari-small-4x8-COLOR1.h
extern tile_struct row_zero[];              // Defined in playfield_and_data.c
extern tile_struct row_one[];               // Defined in playfield_and_data.c
extern tile_struct row_two[];               // Defined in playfield_and_data.c
extern tile_struct row_three[];             // Defined in playfield_and_data.c
extern uint8_t* playfield_lut[PLAYFIELD_ROWS*PLAYFIELD_TILES_ROWS];
extern Player players[MAX_PLAYERS];

// Globals
uint16_t addr;

void wait_for_start_input()
{
    cgetc();
}

void show_logo_splash()
{
    // Initialize graphics mode
    OS.sdlst = &display_list_anticF;   // Show the logo
    OS.gprior |= GFX_9;                // Enable GTIA (graphics 9)
    
    wait_for_start_input();            // Wait for a key press
    
    // Disable graphics 9, returns to graphics 8
    OS.gprior &= ~GFX_9;               // Disable GTIA (graphics 9)
}

void init_graphics()
{
    //int i;    // Temp

    saved_graphics_state.sdmctl = OS.sdmctl; // Save the current SDMCTL register state
    saved_graphics_state.sdlist = (void*)0xbc20;    // Basic was enabled... need to do something different.  OS.sdlst;  // Save the current display list
    saved_graphics_state.vdslst = OS.vdslst; // Save the current vertical display list
    saved_graphics_state.nmien  = ANTIC.nmien; // Save the NMI state
    saved_graphics_state.gprior = OS.gprior; // Save the graphics priority state
    saved_graphics_state.pcolr0 = OS.pcolr0; // Save player-missile 0 color
    saved_graphics_state.pcolr1 = OS.pcolr1; // Save player-missile 1 color
    saved_graphics_state.pcolr2 = OS.pcolr2; // Save player-missile 2 color
    saved_graphics_state.pcolr3 = OS.pcolr3; // Save player-missile 3 color
    saved_graphics_state.color0 = OS.color0; // Save playfield 0 color
    saved_graphics_state.color1 = OS.color1; // Save playfield 1 color
    saved_graphics_state.color2 = OS.color2; // Save playfield 2 color
    saved_graphics_state.color3 = OS.color3; // Save playfield 3 color
    saved_graphics_state.color4 = OS.color4; // Save background color
    saved_graphics_state.chbas  = OS.chbas;  // Save original font location

    show_logo_splash();
    
    // Disable ANTIC while we clear the playfield memory
    OS.sdmctl = 0x00; // Disable ANTIC 
    memset(row_zero,  0x00, sizeof(playfield_block_struct)); // Clear row zero playfield data
    memset(row_one,   0x00, sizeof(playfield_block_struct)); // Clear row one playfield data
    memset(row_two,   0x00, sizeof(playfield_block_struct)); // Clear row two playfield data
    memset(row_three, 0x00, sizeof(playfield_block_struct)); // Clear row three playfield data
    load_map(); // Load the playfield map data
    //cgetc(); // Wait for a key press

    // Use our font for color text
    OS.chbas = (uint8_t)((uintptr_t)font >> 8); // Set the character base address to our font

    // Temp
    //memset((void*)0xE000, 0xAA, 128); // Clear row three playfield data
    //for(i=0; i<256; ++i)
    //    row_zero[0].data[0%40][i] = i; // Clear the first tile in row zero

    // Change to ANTIC 4
    OS.sdlst = &display_list_antic4;         // Set the display list to ANTIC 4 mode

    // Set the graphics colors
    OS.pcolr0 = 14;         // Player-missile 0 color (maximum luminance)
    OS.pcolr1 = 14;         // Player-missile 1 color (maximum luminance)
    OS.pcolr2 = 14;         // Player-missile 2 color (maximum luminance)
    OS.pcolr3 = 14;         // Player-missile 3 color (maximum luminance)
    OS.color0 = 0x0F;       // Playfield 0 color (maximum luminance)    // White
    // OS.color1 = 0x0C;       // Playfield 1 color (maximum luminance)
    // OS.color2 = 0x08;       // Playfield 2 color (maximum luminance)
    // OS.color3 = 0x03;       // Playfield 3 color (maximum luminance)
    OS.color1 = 0x49;       // Playfield 1 color (maximum luminance)    // Red
    OS.color2 = 0xC9;       // Playfield 2 color (maximum luminance)    // Green
    OS.color3 = 0x99;       // Playfield 3 color (maximum luminance)    // Blue
    OS.color4 = 0x00;       // Background color (black)

    // Enable our scrolling vbi
    enable_scroll_vbi();

    OS.sdmctl = 0x22;       // Enable ANTIC (normal playfield width)
    //OS.sdmctl = 0x23;     // Enable ANTIC (wide playfield width)
    //ANTIC.hscrol = 0x0F;    // Set horizontal scroll to 0
}

void render_frame()
{
    // The hardware is taking care of the rendering for us, so we just need to update the display list with the scrolling information.
    // We move the playfield based on the player position.
    // Don't do anything if the player is not moving.
    if (players[PLAYER_ONE].pos.x != players[PLAYER_ONE].pos_prev.x || players[PLAYER_ONE].pos.y != players[PLAYER_ONE].pos_prev.y) {
        scroll_flag = true; // Set the scroll flag to true, indicating that we need to update the display list
    #if 0
        uint16_t i;
        uint8_t* dl = (uint8_t*)&display_list_antic4; // Pointer to the display list
        //uint16_t addr;

        // Update the display list with the new player position (fine scrolling using ANTIC registers)
        ANTIC.vscrol = 0x0F - (uint8_t)(players[PLAYER_ONE].pos.y & 0x0F); // Vertical scroll based on player Y position
        ANTIC.hscrol = 0x0F - (uint8_t)(players[PLAYER_ONE].pos.x & 0x0F); // Horizontal scroll based on player X position

        // Course scrolling using the display list
        // Update the display list with the new player position (course scrolling using the display list)
        //addr = playfield_lut[players[PLAYER_ONE].pos.y >> 8] + (players[PLAYER_ONE].pos.x >> 8);
        if (0 == players[PLAYER_ONE].pos.y & 0x0F) {
            for (i = 0; i < PLAYFIELD_ROWS; ++i) {
                addr = playfield_lut[(players[PLAYER_ONE].pos.y >> 4)+i];

                // Update the display list with the new player position
                // The playfield_lut is a lookup table for the playfield data
                // We use the player position to determine which row to use
                dl[i*3 + 4] = (uint8_t)(addr & 0xFF); // Set the low byte of the address
                dl[i*3 + 5] = (uint8_t)((addr >> 4) & 0xFF); // Set the high byte of the address
                //printf("%d|%d|%d|%04X:%02X  %02X|%02X  %02X\n", players[PLAYER_ONE].pos.y, i, (players[PLAYER_ONE].pos.y >> 4), addr, (uint8_t)(addr & 0xFF), (uint8_t)((addr >> 4) & 0xFF), dl[i*3 + 4], dl[i*3 + 5]); // Debugging output
            }
        }
    #endif
        players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store the previous position
    }
}

void shutdown_graphics()
{
    // Disable the scrolling VBI
    disable_scroll_vbi();

    // Disable ANTIC
    OS.sdmctl = 0x00; // Disable ANTIC

    // Restore the starting graphics mode
    OS.chbas  = saved_graphics_state.chbas;  // Restore the original character base address
    OS.pcolr0 = saved_graphics_state.pcolr0; // Restore player-missile 0 color
    OS.pcolr1 = saved_graphics_state.pcolr1; // Restore player-missile 1 color
    OS.pcolr2 = saved_graphics_state.pcolr2; // Restore player-missile 2 color
    OS.pcolr3 = saved_graphics_state.pcolr3; // Restore player-missile 3 color
    OS.color0 = saved_graphics_state.color0; // Restore playfield 0 color
    OS.color1 = saved_graphics_state.color1; // Restore playfield 1 color
    OS.color2 = saved_graphics_state.color2; // Restore playfield 2 color
    OS.color3 = saved_graphics_state.color3; // Restore playfield 3 color
    OS.color4 = saved_graphics_state.color4; // Restore background color
    OS.gprior = saved_graphics_state.gprior; // Restore the graphics priority state
    ANTIC.nmien = saved_graphics_state.nmien;// Restore the NMI state
    OS.vdslst = saved_graphics_state.vdslst; // Restore the saved vertical display list
    OS.sdlst = saved_graphics_state.sdlist;  // Restore the saved display list
    OS.sdmctl = saved_graphics_state.sdmctl; // Restore the SDMCTL register state
}
