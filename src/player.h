#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "shapes.h"

#include <stdint.h>

typedef struct _Player {
    Point pos;
    uint8_t health;
    uint8_t armor;
    uint8_t ammo;
    uint8_t score;
} Player;

#endif // __PLAYER_H__