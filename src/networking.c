// (C) 2025 Brad Colbert

#include "player.h"

#include <stdio.h>

// Externals
extern Player players[MAX_PLAYERS];

void send_player_data()
{
    #if 0
    printf("%d %d\n", // %d %d %d %d %d\n",
           players[PLAYER_ONE].pos.x,
           players[PLAYER_ONE].pos.y/*,
           players[PLAYER_ONE].direction,
           players[PLAYER_ONE].health,
           players[PLAYER_ONE].armor,
           players[PLAYER_ONE].ammo,
           players[PLAYER_ONE].score*/
        );
    #endif
}

void receive_game_data()
{
}