// (C) 2025 Brad Colbert
#ifndef __DISPLAY_LIST_ANTIC4_H__
#define __DISPLAY_LIST_ANTIC4_H__

#include "playfield.h"

#include <atari.h>

// Defines
#define TILE_NUM 0

// Globals
extern tile_struct row_zero[]; // 4080 bytes for the first part (102 lines of 40 bytes each)

void display_list_antic4 =
{
    DL_BLK8,
    DL_BLK8,
    DL_BLK8,

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[0][0], // ANTIC 4
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[1][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[2][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[3][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[4][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[5][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[6][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[7][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[8][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[9][0], // Ln 10,  33 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[10][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[11][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[12][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[13][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[14][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[15][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[16][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[17][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[18][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[19][0], // Ln 20,  63 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[20][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[21][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[22][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero[TILE_NUM].data[23][0], // Ln 24,  75 bytes

    DL_LMS(DL_HSCROL(DL_CHR40x8x4)),  &row_zero[TILE_NUM].data[24][0],            // Buffer Ln 25,  75 bytes

    DL_JVB, &display_list_antic4,                                                 // 78 bytes

    // A bunch of blanks to fill the rest of the display list memory (to 256 bytes)
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 79-88
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 89-98
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 99-108
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 109-118
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 119-128
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 129-138
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 139-148
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 149-158
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 159-168
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 169-178
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-188
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-198
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-208
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-218
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-228
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-238
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8,   // 179-248
    DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8, DL_BLK8                                                 // 179-255
};

#endif // __DISPLAY_LIST_ANTIC4_H__