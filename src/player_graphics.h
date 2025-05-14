// Define code for your platform
#ifndef __PLAYER_GRAPHICS_H__
#define __PLAYER_GRAPHICS_H__

#include "player.h"

#define PLAYER_HEIGHT 256
#define PLAYER_BLANK (2048 - (PLAYER_HEIGHT * 5))
#define PLAYER_LINE_MODE DMACTL_PMG_SINGLELINE

typedef struct {
    uint8_t blank[768]; //PLAYER_BLANK];
    uint8_t missile3210[PLAYER_HEIGHT];
    uint8_t player0[PLAYER_HEIGHT];
    uint8_t player1[PLAYER_HEIGHT];
    uint8_t player2[PLAYER_HEIGHT];
    uint8_t player3[PLAYER_HEIGHT];
} PlayerMissiles;

void __fastcall__ init_player_graphics(Player* player);
void __fastcall__ update_player_graphics(Player* player);
void __fastcall__ draw_player_graphics(Player* player);
void __fastcall__ close_player_graphics(void);

#endif // __PLAYER_GRAPHICS_H__