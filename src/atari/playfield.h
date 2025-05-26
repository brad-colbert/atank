// (C) 2025 Brad Colbert
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include <stdint.h>

typedef struct tile_s {
    uint8_t data[24][40]; // 1000 bytes per tile
} tile_struct;

#endif // __PLAYFIELD_H__