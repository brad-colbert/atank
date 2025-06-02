// (C) 2025 Brad Colbert

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "shapes.h"

#include <stdint.h>

// Defines
#define MAX_PLAYERS 4
#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define PLAYER_THREE 2
#define PLAYER_FOUR 3

typedef struct _Player {
    Point pos_prev;
    Point pos;
    uint8_t direction; // 0: up, 1: up-right, 2: right, 3: down-right, 4: down, 5: down-left, 6: left, 7: up-left
    uint8_t health;
    uint8_t armor;
    uint8_t ammo;
    uint8_t score;
} Player;

void init_player(int16_t x, int16_t y);

#endif // __PLAYER_H__