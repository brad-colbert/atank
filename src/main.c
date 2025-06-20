// (C) 2025 Brad Colbert
#include "input.h"
#include "networking.h"
#include "game_state.h"
#include "graphics.h"

#include <conio.h>

#include <stdint.h>
#include <stdbool.h>

// Globals
bool done = false;

// Imports
void main(void)
{
    // Initialize input handling
    setup_input();

    // Initialize the player data
    init_player(10, 10); // Starting position for player one

    // Initialize graphics
    init_graphics();

    while(!kbhit())
    {
        // Get user input
        process_input();
        
        // Send player input/state to the server
        send_player_data();

        // Process incoming data
        receive_game_data();

        // Update the game state
        update_game_state();

        // Interpolate the game state
        interpolate_game_state();

        // Render the frame
        render_frame();
    }

    // Restore the starting graphics mode
    shutdown_graphics();
}