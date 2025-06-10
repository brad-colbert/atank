// (C) 2025 Brad Colbert

#include "graphics.h"
#include "playfield.h"
#include "player.h"
#include "player_missiles.h"
#include "player_graphics_data.h"
#include "memory.h"
#include "console_keys.h"

#include <atari.h>
#include <conio.h>
#include <joystick.h>

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
extern graphics_state saved_graphics_state; // Defined in playfield_and_data.c
extern uint8_t font[];                      // Defined in atari-small-4x8-COLOR1.h
extern playfield_block_struct row_zero;              // Defined in playfield_and_data.c
extern playfield_block_struct row_one;               // Defined in playfield_and_data.c
extern playfield_block_struct row_two;               // Defined in playfield_and_data.c
extern playfield_block_struct row_three;             // Defined in playfield_and_data.c
extern uint8_t* playfield_lut[PLAYFIELD_ROWS*PLAYFIELD_TILES_ROWS];
extern uint8_t player_count;                // Defined in player.c 
extern Player players[MAX_PLAYERS];
extern bool scroll_flag;                    // Defined in playfield_scroll.s
extern int16_t x_pos_shadow;
extern int16_t y_pos_shadow;
extern PlayerImageData player_0_image_data; // Defined in player_graphics_data.h
extern PlayerMissiles player_missiles;      // Defined in player_missiles.h
extern uint8_t pm_x_coord_map[]; // Defined in player_missiles.c
extern uint8_t pm_y_coord_map[]; // Defined in player_missiles.c

// Globals
uint16_t addr;
uint16_t coord = 0;

void wait_for_start_input()
{
    while(!kbhit() && !read_console_keys(CONSOLE_KEY_START)) // Wait for a key press
    {
        uint8_t joy_state = joy_read(JOY_1);
        if(JOY_BTN_1(joy_state)) // If the joystick button 1 is pressed
        {
            break; // Exit the loop
        }

    }
    if(kbhit()) // If a key was pressed
        cgetc();  // Clear the key press
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
    //#define TESTING
    #ifdef TESTING
    uint8_t row, col;
    #endif

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
    memset(&row_zero,  0x00, sizeof(playfield_block_struct)); // Clear row zero playfield data
    memset(&row_one,   0x00, sizeof(playfield_block_struct)); // Clear row one playfield data
    memset(&row_two,   0x00, sizeof(playfield_block_struct)); // Clear row two playfield data
    memset(&row_three, 0x00, sizeof(playfield_block_struct)); // Clear row three playfield data

    load_map(); // Load the playfield map data

    // Use our font for color text
    OS.chbas = (uint8_t)((uintptr_t)font >> 8); // Set the character base address to our font

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

    // Setup the player missiles
    init_player_missiles();

    #ifdef TESTING
    for(row = 1; row < 23; row++) {
        row_zero.rows[row].cols[0] = ('0'-32) + row%10; // Set the first column to the row number
        if(row % 2)
            row_zero.rows[row].cols[19] = ('0'-32) + 9;
    }
    for(col = 1; col < 39; col++) {
        // Set the playfield tile data
        row_zero.rows[0].cols[col] = ('0'-32) + col%10; // Set the first row to the column number
        if(col % 2)
            row_zero.rows[11].cols[col] = ('0'-32) + 1;
    }
    row_zero.rows[11].cols[20] = 0; // blank
    row_zero.rows[11].cols[19] = '8'; // X
    #endif

    // Force an update of the playfield
    scroll_flag = false; // Set the scroll flag to false
    x_pos_shadow = 0; // Move the playfield in X
    y_pos_shadow = 0; // Store the current y position
    scroll_flag = true; // Set the scroll flag to true, indicating that we need to update the display list
}

void render_frame()
{
    #ifdef TESTING

    // Set the playfield tile data for row one
    #define PMG_W_H 8
    // UL
    #define PMG_X 47 //(55-PMG_W_H)
    #define PMG_Y 32 //(40-PMG_W_H)
    // Center
    // #define PMG_X 121 //(129-PMG_W_H)
    // #define PMG_Y 120 //(128-PMG_W_H)
    // LR
    // #define PMG_X 199 //(211-PMG_W_H)
    // #define PMG_Y 224 //(224-PMG_W_H)
    GTIA_WRITE.hposp0 = PMG_X;
    //memcpy(&(player_missiles.player0[PMG_XY]), player_0_image_data.base_images[2], 8);
    player_missiles.player0[PMG_Y+0] = 0xA5;
    player_missiles.player0[PMG_Y+1] = 0x42;
    player_missiles.player0[PMG_Y+2] = 0xA5;
    player_missiles.player0[PMG_Y+3] = 0x00; //E7;
    player_missiles.player0[PMG_Y+4] = 0x00; //E7;
    player_missiles.player0[PMG_Y+5] = 0xA5;
    player_missiles.player0[PMG_Y+6] = 0x42;
    player_missiles.player0[PMG_Y+7] = 0xA5;
    #else
    //int coord = pm_y_coord_map[128];

    // The hardware is taking care of the rendering for us, so we just need to update the display list with the scrolling information.
    // We move the playfield based on the player position.
    // Don't do anything if the player is not moving.
    if(!scroll_flag) {
        // We update the players previous position here so that we don't spend idle time updating the display list.
        if (players[PLAYER_ONE].pos.x != players[PLAYER_ONE].pos_prev.x || players[PLAYER_ONE].pos.y != players[PLAYER_ONE].pos_prev.y) {

            // if (players[PLAYER_ONE].pos.x > 603) { //(40 * 8 * 3 + 128)) ){
            //     coord = (players[PLAYER_ONE].pos.x - 604) + 128; // Get the x position
            #define X_LIMIT (40 * 4 * 3)
            if (players[PLAYER_ONE].pos.x > X_LIMIT + 80) { //(40 * 8 * 3 + 128)) ){
                coord = players[PLAYER_ONE].pos.x - X_LIMIT; // Get the x position
                coord = pm_x_coord_map[coord]; // Get the x position from the map
                GTIA_WRITE.hposp0 = coord; // Set horizontal position for player 0
            }
            else if (players[PLAYER_ONE].pos.x < 80) { //(40 * 8 * 3 + 128)) ){
                coord = players[PLAYER_ONE].pos.x & 0xFF; // Get the x position
                coord = pm_x_coord_map[coord]; // Get the x position from the map
                GTIA_WRITE.hposp0 = coord; // Set horizontal position for player 0
            }
            else {
                x_pos_shadow = players[PLAYER_ONE].pos.x - 80; // Move the playfield in X
            }

            #define Y_LIMIT (24 * 8 * 3)
            if (players[PLAYER_ONE].pos.y > Y_LIMIT + 95) {
                coord = (players[PLAYER_ONE].pos_prev.y - Y_LIMIT); // Get the y position
                coord = pm_y_coord_map[coord]; // Get the y position from the map
                player_missiles.player0[coord-1] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+0] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+1] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+2] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+3] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+4] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+5] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+6] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+7] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+8] = 0x00; // Clear the previous player graphics

                coord = (players[PLAYER_ONE].pos.y - Y_LIMIT); // Get the x position
                coord = pm_y_coord_map[coord]; // Get the x position from the map
            }
            else if (players[PLAYER_ONE].pos.y < 97) {
                // Clear the previous player graphics
                coord = players[PLAYER_ONE].pos_prev.y & 0x7F; // Get the y position
                coord = pm_y_coord_map[coord]; // Get the y position from the map
                player_missiles.player0[coord-1] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+0] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+1] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+2] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+3] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+4] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+5] = 0x00; // Clear the previous player graphics
                // player_missiles.player0[coord+6] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+7] = 0x00; // Clear the previous player graphics
                player_missiles.player0[coord+8] = 0x00; // Clear the previous player graphics

                coord = players[PLAYER_ONE].pos.y & 0x7F; // Get the y position
                coord = pm_y_coord_map[coord]; // Get the y position from the map
            }
            else {
                y_pos_shadow = players[PLAYER_ONE].pos.y - 96; // Store the current y position
                coord = pm_y_coord_map[96]; // Get the y position from the map
            }

            memcpy(&(player_missiles.player0[coord]), player_0_image_data.base_images[players[PLAYER_ONE].direction], 8);

            players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos;             // Update the previous position
            players[PLAYER_ONE].direction_prev = players[PLAYER_ONE].direction; // Update the previous direction
            
            scroll_flag = true; // Set the scroll flag to true, indicating that we need to update the display list
        }
    }
    #endif
}

void shutdown_graphics()
{
    // Disable the scrolling VBI
    disable_scroll_vbi();

    // Disable ANTIC
    OS.sdmctl = 0x00; // Disable ANTIC

    disable_player_missiles(); // Disable player missiles

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
