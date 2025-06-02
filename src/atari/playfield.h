// (C) 2025 Brad Colbert
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include <stdint.h>

// Defines
#define PLAYFIELD_ROWS 24
#define PLAYFIELD_COLS 40
#define PLAYFIELD_TILES_ROWS 4
#define PLAYFIELD_TILES_COLS 4

typedef struct tile_s {
    uint8_t data[PLAYFIELD_ROWS][PLAYFIELD_COLS]; // 960 bytes per tile
} tile_struct;

typedef struct row_playfield_s {
    uint8_t cols[PLAYFIELD_COLS * PLAYFIELD_TILES_COLS]; // 160 bytes
} row_playfield_struct;

typedef union playfield_block_s {
    tile_struct tiles[PLAYFIELD_TILES_COLS]; // 4 tiles per block 4 * 24 * 40
    row_playfield_struct rows[PLAYFIELD_ROWS]; // 160 * 24 bytes playfield data
} playfield_block_struct;

//  Defined in playfield_scroll.s
void enable_scroll_vbi();
void disable_scroll_vbi();
void test_update_scroll();

#endif // __PLAYFIELD_H__