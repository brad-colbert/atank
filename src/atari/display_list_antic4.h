// (C) 2025 Brad Colbert
#ifndef __DISPLAY_LIST_ANTIC4_H__
#define __DISPLAY_LIST_ANTIC4_H__

#include <atari.h>

#pragma bss-name (push,"DISPLAYLIST")
void display_list_antic4 =
{
    DL_BLK8,
    DL_BLK8,
    DL_BLK8,

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[0][0], // ANTIC 4
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[1][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[2][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[3][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[4][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[5][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[6][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[7][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[8][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[9][0], // Ln 10,  33 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[10][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[11][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[12][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[13][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[14][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[15][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[16][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[17][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[18][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[19][0], // Ln 20,  63 bytes

    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[20][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[21][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[22][0],
    DL_LMS(DL_VSCROL(DL_HSCROL(DL_CHR40x8x4))),  &playfield[23][0], // Ln 24,  75 bytes

    DL_LMS(DL_HSCROL(DL_CHR40x8x4)),  &playfield[24][0],            // Buffer Ln 25,  75 bytes

    DL_JVB, &display_list_antic4                                    // 78 bytes
};
#pragma bss-name (pop)

#endif // __DISPLAY_LIST_ANTIC4_H__