#ifndef __PLAYER_MISSILES_H__
#define __PLAYER_MISSILES_H__

#include "types.h"

#define PMG_SINGLE_LINE
#define PLAYER_WIDTH 8
#ifdef PMG_SINGLE_LINE
#define PLAYER_HEIGHT 256
#define PLAYER_BLANK (2048 - (PLAYER_HEIGHT * 5))
#define PLAYER_LINE_MODE DMACTL_PMG_SINGLELINE
#else
#define PLAYER_HEIGHT 128
#define PLAYER_BLANK (1024 - (PLAYER_HEIGHT * 5))
#define PLAYER_LINE_MODE 0
#endif

typedef struct {
    char blank[PLAYER_BLANK];
    char missile3210[PLAYER_HEIGHT];
    char player0[PLAYER_HEIGHT];
    char player1[PLAYER_HEIGHT];
    char player2[PLAYER_HEIGHT];
    char player3[PLAYER_HEIGHT];
} PlayerMissiles;

typedef struct
{
    byte dirty;
    u_short x, y;  // absolute position relative to the playfield
    char image_idx, anim_dir;
    byte image_time;
} Player;

#define MAX_PLAYERS 4
union __players {
    Player all[MAX_PLAYERS];
    struct {
        Player one;
        Player two;
        Player three;
        Player four;
    };
};

typedef union __players Players;
extern Players players;

extern byte num_players;
#pragma zpsym("num_players")

#define PF_MIN_X 48
#define PF_MAX_X 200
#ifdef PMG_SINGLE_LINE
#define PF_MIN_Y 31
#define PF_MAX_Y 215
#else
#define PF_MIN_Y 10
#define PF_MAX_Y 110
#endif

void set_player_position(byte idx, byte x, byte y);
void init_player_missiles(void);
void update_player_missiles(void);
void close_player_missiles(void);

#endif // __PLAYER_MISSILES_H__
