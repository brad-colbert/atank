#include "player.h"
#include "graphics.h"

#include <cc65.h>

// Externals

// Globals
int16_t angle = 0;

void init_player(Player* player, int16_t x, int16_t y)
{
    player->pos.x = x;
    player->pos.y = y;
    player->health = 100;
    player->armor = 50;
    player->ammo = 30;
    player->score = 0;
}

void update_player(Player* player)
{
    // Update player position based on input or game logic
    // This will move the player position around in a circle
    player->pos.x = _sin(angle)/2 + 225; //+ 250 + WIDTH_PIXELS/4;
    player->pos.y = _cos(angle)/2 + 150; //+ 96 + HEIGHT_PIXELS/4;
    angle += 1;
    if(angle > 360)
        angle = 0;
    
    // Update player health, armor, ammo, and score as needed
    // For example, decrease health by 1
    if (player->health > 0) {
        player->health--;
    }
}
