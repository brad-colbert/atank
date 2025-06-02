// (C) 2025 Brad Colbert
#ifndef __DISPLAY_LIST_ANTIC4_H__
#define __DISPLAY_LIST_ANTIC4_H__

#include "playfield.h"

#include <atari.h>

// Defines
#define TILE_NUM 0

// Imports
extern playfield_block_struct row_zero; // 4080 bytes for the first part (102 lines of 40 bytes each)
extern playfield_block_struct row_one; // 4080 bytes for the first part (102 lines of 40 bytes each)

void display_list_antic4 =
{
    DL_BLK8,
    DL_BLK8,
    DL_BLK8,

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[0],  // ANTIC 4
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[1],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[2],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[3],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[4],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[5],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[6],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[7],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[8],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[9],  // Ln 10,  33 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[10],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[11],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[12],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[13],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[14],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[15],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[16],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[17],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[18],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[19], // Ln 20,  63 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[20],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[21],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[22],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &row_zero.rows[23], // Ln 24,  75 bytes

    //DL_LMS(DL_HSCROL(DL_CHR40x8x4)),  &row_one.rows[0],              // Buffer Ln 25,  75 bytes

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