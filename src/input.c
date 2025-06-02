// (C) 2025 Brad Colbert

#include "input.h"
#include "player.h"

#include <joystick.h>

// Imports
extern Player players[MAX_PLAYERS];

void setup_input()
{
    // Initialize the joystick
    joy_install(joy_static_stddrv);
}

// We are using the generic joystick input handling provided by CC65 for ease of porting.
// For now, we only control one player since this is a networked game and we assume one player per console.
// In development, at least for now, the motion mechanics will be simple but we will be moving to a dynamic model (velocity, acceleration, etc.) later on.
void process_input()
{
    uint8_t joy_state = joy_read(JOY_1);

    // Check joystick input
    if (JOY_UP(joy_state)) {
        players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        if(JOY_LEFT(joy_state)) {
            players[PLAYER_ONE].direction = 7; // Up-Left
            players[PLAYER_ONE].pos.x -= 1;
            players[PLAYER_ONE].pos.y -= 1;
        } else if (JOY_RIGHT(joy_state)) {
            players[PLAYER_ONE].direction = 1; // Up-Right
            players[PLAYER_ONE].pos.x += 1;
            players[PLAYER_ONE].pos.y -= 1;
        } else {
            players[PLAYER_ONE].direction = 0; // Up
            players[PLAYER_ONE].pos.y -= 1;
        }
    }
    else if (JOY_DOWN(joy_state)) {
        players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        if(JOY_LEFT(joy_state)) {
            players[PLAYER_ONE].direction = 5; // Down-Left
            players[PLAYER_ONE].pos.x -= 1;
            players[PLAYER_ONE].pos.y += 1;
        } else if (JOY_RIGHT(joy_state)) {
            players[PLAYER_ONE].direction = 3; // Down-Right
            players[PLAYER_ONE].pos.x += 1;
            players[PLAYER_ONE].pos.y += 1;
        } else {
            players[PLAYER_ONE].direction = 4; // Down
            players[PLAYER_ONE].pos.y += 1;
        }
    }
    else if (JOY_LEFT(joy_state)) {
        players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        players[PLAYER_ONE].direction = 6; // Left
        players[PLAYER_ONE].pos.x -= 1;
    }
    else if (JOY_RIGHT(joy_state)) {
        players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        players[PLAYER_ONE].direction = 2; // Right
        players[PLAYER_ONE].pos.x += 1;
    }

    // Apply bounds checking to prevent the player from moving off-screen
    if (players[PLAYER_ONE].pos.x < 0) {
        players[PLAYER_ONE].pos.x = 0;
    } else if (players[PLAYER_ONE].pos.x > 2560-640) {
        players[PLAYER_ONE].pos.x = 2560-640;
    }
    if (players[PLAYER_ONE].pos.y < 0) {
        players[PLAYER_ONE].pos.y = 0;
    } else if (players[PLAYER_ONE].pos.y > 1536-384) { // Assuming a screen height of 192 pixels
        players[PLAYER_ONE].pos.y = 1536-384;
    }
}