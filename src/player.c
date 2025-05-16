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

uint8_t test_collision(Player* player)
{
// Quick check on collision
    if(GTIA_READ.p0pf)
    {
        switch(GTIA_READ.p0pf)
        {
            case 0x01:
                OS.pcolr0 = GTIA_COLOR_WHITE;
            break;
            case 0x02:
                OS.pcolr0 = GTIA_COLOR_GREEN;
            break;
            case 0x04:
                OS.pcolr0 = GTIA_COLOR_BROWN;
            break;
        }

        GTIA_WRITE.hitclr = 0x01;

        return 0x01;
    }
    //else
    //    OS.pcolr0 = GTIA_COLOR_LIGHTRED;

    return 0x00;
}

void update_player(Player* player)
{
    uint8_t joy_state = joy_read(JOY_1);

    if(test_collision(player))
    {
        // player->health -= 1;
        // if (player->health == 0) {
        //     // Handle player death
        //     player->pos.x = 0;
        //     player->pos.y = 0;
        // }
        player->pos = player->pos_prev; // Reset to previous position
    }
    else
    {

    // Check joystick input
    if (JOY_UP(joy_state)) {
        player->pos_prev = player->pos; // Store previous position
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
        player->pos_prev = player->pos; // Store previous position
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
        player->pos_prev = player->pos; // Store previous position
        player->direction = 6; // Left
        player->pos.x -= 1;
    }
    else if (JOY_RIGHT(joy_state)) {
        player->pos_prev = player->pos; // Store previous position
        player->direction = 2; // Right
        player->pos.x += 1;
    }
    }

    if (JOY_FIRE(joy_state)) {
        // Fire weapon or perform action
        player->ammo--;
    }

    update_player_graphics(player);
}
