#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "shapes.h"

#include <stdint.h>

typedef struct _Player {
    Point pos;
    uint8_t direction; // 0: up, 1: up-right, 2: right, 3: down-right, 4: down, 5: down-left, 6: left, 7: up-left
    uint8_t health;
    uint8_t armor;
    uint8_t ammo;
    uint8_t score;
} Player;

void __fastcall__ init_player(Player* player, int16_t x, int16_t y);
//void __fastcall__ move_player(Player* player, int16_t dx, int16_t dy);
//void __fastcall__ draw_player(Player* player);
void __fastcall__ update_player(Player* player);

#endif // __PLAYER_H__