// (C) 2025 Brad Colbert
#include "player_missiles.h"
#include "player_graphics_data.h"
#include "player.h"

#include <atari.h>

#include <stdint.h>
#include <string.h>

// Imports
extern uint8_t player_count;                // Defined in player.c 
extern Player players[MAX_PLAYERS];         // Defined in player.c
extern PlayerImageData player_0_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_1_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_2_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_3_image_data; // Defined in player_graphics_data.h
extern PlayerMissiles player_missiles;      // Defined in player_missiles.h
extern uint8_t pm_x_coord_map[]; // Defined in player_missiles.c
extern uint8_t pm_y_coord_map[]; // Defined in player_missiles.c

#define X_MIN 46
#define X_MAX 201
#define X_DELTA (X_MAX - X_MIN) // 155
#define Y_MIN 31
#define Y_MAX 217
#define Y_DELTA (Y_MAX - Y_MIN) // 186


void update_players()
{
    #if 0
    uint8_t i, y;

    // Update the player's previous position done when the graphics are rendered in render_frame();
    
    // Update the player's current position, done in input.c

    // Update the player's direction, done in input.c

    // Update the players graphics
    for(i = 0; i < player_count; i++) {
        switch(i) {
            case PLAYER_ONE:
                // Clear the previous player graphics
                y = players[i].pos_prev.y & 0xFF; // Get the y position
                y = pm_x_coord_map[y]; // Get the y position from the map
                memset(&(player_missiles.player0[y]), 0x00, 8); // Clear the previous player graphics
                y = players[i].pos.y & 0xFF; // Get the y position
                y = pm_x_coord_map[y]; // Get the y position from the map
                memcpy(&(player_missiles.player0[y]), player_0_image_data.base_images[players[i].direction], 8);
                y = players[i].pos.x & 0xFF; // Get the x position
                y = pm_x_coord_map[y]; // Get the x position from the map
                GTIA_WRITE.hposp0 = y; // Set horizontal position for player 0
                break;
            case PLAYER_TWO:
                memcpy(&(player_missiles.player1[127-4]), player_1_image_data.base_images[players[i].direction], 8);
                GTIA_WRITE.hposp1 = 0;
                break;
            case PLAYER_THREE:
                memcpy(&(player_missiles.player2[127-4]), player_2_image_data.base_images[players[i].direction], 8);
                GTIA_WRITE.hposp2 = 0;
                break;
            case PLAYER_FOUR:
                memcpy(&(player_missiles.player3[127-4]), player_3_image_data.base_images[players[i].direction], 8);
                GTIA_WRITE.hposp3 = 0;
                break;
        }
    }
    #endif
}