#include "player.h"
#include "graphics.h"
#include "player_graphics.h"

#include <joystick.h>

// Externals

// Globals

void init_player(Player* player, int16_t x, int16_t y)
{
    player->pos.x = x;
    player->pos.y = y;
    player->health = 100;
    player->armor = 50;
    player->ammo = 30;
    player->score = 0;

    init_player_graphics(player);
}

void update_player(Player* player)
{
    uint8_t joy_state = joy_read(JOY_1);

    // Check joystick input
    if (JOY_UP(joy_state)) {
        if(JOY_LEFT(joy_state)) {
            player->direction = 7; // Up-Left
            player->pos.x -= 1;
            player->pos.y -= 1;
        } else if (JOY_RIGHT(joy_state)) {
            player->direction = 1; // Up-Right
            player->pos.x += 1;
            player->pos.y -= 1;
        } else {
            player->direction = 0; // Up
            player->pos.y -= 1;
        }
    }
    else if (JOY_DOWN(joy_state)) {
        if(JOY_LEFT(joy_state)) {
            player->direction = 5; // Down-Left
            player->pos.x -= 1;
            player->pos.y += 1;
        } else if (JOY_RIGHT(joy_state)) {
            player->direction = 3; // Down-Right
            player->pos.x += 1;
            player->pos.y += 1;
        } else {
            player->direction = 4; // Down
            player->pos.y += 1;
        }
    }
    else if (JOY_LEFT(joy_state)) {
        player->direction = 6; // Left
        player->pos.x -= 1;
    }
    else if (JOY_RIGHT(joy_state)) {
        player->direction = 2; // Right
        player->pos.x += 1;
    }
    if (JOY_FIRE(joy_state)) {
        // Fire weapon or perform action
        player->ammo--;
    }

    update_player_graphics(player);
}
