#define DEBUG

#include "map.h"
#include "graphics.h"
#include "player.h"

#include <conio.h>

#include <stdio.h>
#include <stdint.h>

// Externals
extern uint8_t line_count;
extern Line lines[];
extern Point map_center;

// Globals
Player player;

void main(void)
{
    // Load the map
    load_map("map.txt", &line_count);

    #ifdef DEBUG
    fprintf(stderr, "Line count: %d\n", line_count);
    #endif

    // Setup the players
    init_player(&player, 250, 150);

    // Initialize graphics
    init_graphics();

    // Clear the screen
    clear_graphics();

    while(!kbhit()) {
        update_player(&player);
        set_map_center(&player.pos);
        draw_map();
    }
    cgetc();  // Clear the key buffer

    // Return to previous mode
    shutdown_graphics();

    cgetc();  // Wait for a key press to exit
}