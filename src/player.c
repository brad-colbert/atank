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
        player->pos.y -= 1;
    }
    if (JOY_DOWN(joy_state)) {
        player->pos.y += 1;
    }
    if (JOY_LEFT(joy_state)) {
        player->pos.x -= 1;
    }
    if (JOY_RIGHT(joy_state)) {
        player->pos.x += 1;
    }
    if (JOY_FIRE(joy_state)) {
        // Fire weapon or perform action
        player->ammo--;
    }

    update_player_graphics(player);
}
