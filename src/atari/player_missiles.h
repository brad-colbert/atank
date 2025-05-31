// (C) 2025 Brad Colbert

#ifndef __PLAYER_GRAPHICS_H__
#define __PLAYER_GRAPHICS_H__

#include <stdint.h>

// Defines
#define PLAYER_HEIGHT 256
#define PLAYER_BLANK (2048 - (PLAYER_HEIGHT * 5))
#define PLAYER_LINE_MODE DMACTL_PMG_SINGLELINE

typedef struct {
    uint8_t blank[PLAYER_BLANK];
    uint8_t missile3210[PLAYER_HEIGHT];
    uint8_t player0[PLAYER_HEIGHT];
    uint8_t player1[PLAYER_HEIGHT];
    uint8_t player2[PLAYER_HEIGHT];
    uint8_t player3[PLAYER_HEIGHT];
} PlayerMissiles;

#endif // __PLAYER_GRAPHICS_H__