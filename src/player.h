// (C) 2025 Brad Colbert

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdint.h>

typedef struct _Player {
    uint8_t direction; // 0: up, 1: up-right, 2: right, 3: down-right, 4: down, 5: down-left, 6: left, 7: up-left
    uint8_t health;
    uint8_t armor;
    uint8_t ammo;
    uint8_t score;
} Player;

#endif // __PLAYER_H__