// (C) 2025 Brad Colbert

#include "player.h"

// Globals
uint8_t player_count = 1; // Start with one player
Player players[MAX_PLAYERS];

// Others will be updated by the network code
void init_player(int16_t x, int16_t y)
{
    players[PLAYER_ONE].pos_prev.x = -1; // Force an initial update
    players[PLAYER_ONE].pos_prev.y = -1;
    players[PLAYER_ONE].pos.x = x;
    players[PLAYER_ONE].pos.y = y;
    players[PLAYER_ONE].health = 100;
    players[PLAYER_ONE].armor = 50;
    players[PLAYER_ONE].ammo = 30;
    players[PLAYER_ONE].score = 0;
}

