// (C) 2025 Brad Colbert

#include "input.h"
#include "player.h"
#include "time.h"

#include <joystick.h>

// Imports
extern Player players[MAX_PLAYERS];
extern uint32_t time_millis;

// Globals
// REMOVE THIS CODE.  HERE TO TEST JOYSTICK INPUT AND TIME KEEPING
uint32_t start_time = 0x12345678; // Initialize to a non-zero value to avoid uninitialized access
uint32_t delta_t; // Calculate the time since the last input processing
int16_t x_dir = 1;
int16_t y_dir = 0;

void setup_input()
{
    // Initialize the joystick
    joy_install(joy_static_stddrv);

    // REMOVE THIS CODE.  HERE TO TEST JOYSTICK INPUT AND TIME KEEPING
    get_time_millis(); // Store the start time in milliseconds
    start_time = time_millis; // Initialize the start time
}

// We are using the generic joystick input handling provided by CC65 for ease of porting.
// For now, we only control one player since this is a networked game and we assume one player per console.
// In development, at least for now, the motion mechanics will be simple but we will be moving to a dynamic model (velocity, acceleration, etc.) later on.
void process_input()
{  
    // REMOVE THIS CODE.  HERE TO TEST TIME KEEPING
    #if 0
    get_time_millis(); // Store the start time in milliseconds
    delta_t = time_millis - start_time; // Calculate the time since the last input processing

    if (delta_t > (uint32_t)1) { // If less than 100 milliseconds, skip processing
        players[PLAYER_ONE].direction = 2; // Right
        players[PLAYER_ONE].pos.x += x_dir;
        players[PLAYER_ONE].direction = 4; // Down
        players[PLAYER_ONE].pos.y += y_dir;
        start_time = time_millis; // Update the start time
    }

    // Apply bounds checking to prevent the player from moving off-screen
    if (players[PLAYER_ONE].pos.x < (int16_t)0) {
        players[PLAYER_ONE].pos.x = (int16_t)0;
        x_dir = 0;
        y_dir = -1;
    } else if (players[PLAYER_ONE].pos.x > (int16_t)(40*4*3-1)) { //2559-640) {
        players[PLAYER_ONE].pos.x = (int16_t)(40*4*3-1); //2559-640;
        x_dir = 0;
        y_dir = 1;
    }
    if (players[PLAYER_ONE].pos.y < (int16_t)0) {
        players[PLAYER_ONE].pos.y = (int16_t)0;
        x_dir = 1;
        y_dir = 0;
    } else if (players[PLAYER_ONE].pos.y > (int16_t)(24 * 8 * 3)) {//(int16_t)1535-384) { // Assuming a screen height of 192 pixels
        players[PLAYER_ONE].pos.y = (int16_t)(24 * 8 * 3); //(int16_t)1535-384;
        x_dir = -1;
        y_dir = 0;
    }
    #else
    uint8_t joy_state = joy_read(JOY_1);

    // REMOVE THIS CODE.  HERE TO TEST JOYSTICK INPUT AND TIME KEEPING
    get_time_millis(); // Store the start time in milliseconds
    delta_t = time_millis - start_time; // Calculate the time since the last input processing

    if (delta_t > (uint32_t)1) { // If less than 100 milliseconds, skip processing
    // Check joystick input
    if (JOY_UP(joy_state)) {
        //players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
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
        //players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
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
        //players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        players[PLAYER_ONE].direction = 6; // Left
        players[PLAYER_ONE].pos.x -= 1;
    }
    else if (JOY_RIGHT(joy_state)) {
        //players[PLAYER_ONE].pos_prev = players[PLAYER_ONE].pos; // Store previous position
        players[PLAYER_ONE].direction = 2; // Right
        players[PLAYER_ONE].pos.x += 1;
    }
        start_time = time_millis; // Update the start time
    }
    #endif

    // Apply bounds checking to prevent the player from moving off-screen
    if (players[PLAYER_ONE].pos.x < (int16_t)0) {
        players[PLAYER_ONE].pos.x = (int16_t)0;
    } else if (players[PLAYER_ONE].pos.x > (int16_t)(40 * 4 * 4 - 8)) {//  (731)) { //(40*8*3-1)) {
        players[PLAYER_ONE].pos.x = (int16_t)(40 * 4 * 4 - 8); //(731); //(40*8*3-1);
    }
    if (players[PLAYER_ONE].pos.y < (int16_t)0) {
        players[PLAYER_ONE].pos.y = (int16_t)0;
    } else if (players[PLAYER_ONE].pos.y > (int16_t)(24 * 8 * 4 - 8)) { //704 ) {//
        players[PLAYER_ONE].pos.y = (int16_t)(24 * 8 * 4 - 8);
    }
}