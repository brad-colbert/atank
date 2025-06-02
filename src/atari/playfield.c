// (C) 2025 Brad Colbert

#include "playfield.h"

// Imports
extern playfield_block_struct row_zero; // 4080 bytes for the first part (102 lines of 40 bytes each)
extern playfield_block_struct row_one; // 4080 bytes for the first part (102 lines of 40 bytes each)
extern playfield_block_struct row_two; // 4080 bytes for the first part (102 lines of 40 bytes each)
extern playfield_block_struct row_three; // 4080 bytes for the first part (102 lines of 40 bytes each)

// Globals
// LUT for playfield data, used to map tile data to the playfield
uint8_t* playfield_lut[PLAYFIELD_ROWS*PLAYFIELD_TILES_ROWS] = {
    &row_zero.rows[0].cols[0], &row_zero.rows[1].cols[0], &row_zero.rows[2].cols[0], &row_zero.rows[3].cols[0],
    &row_zero.rows[4].cols[0], &row_zero.rows[5].cols[0], &row_zero.rows[6].cols[0], &row_zero.rows[7].cols[0],
    &row_zero.rows[8].cols[0], &row_zero.rows[9].cols[0], &row_zero.rows[10].cols[0], &row_zero.rows[11].cols[0],
    &row_zero.rows[12].cols[0], &row_zero.rows[13].cols[0], &row_zero.rows[14].cols[0], &row_zero.rows[15].cols[0],
    &row_zero.rows[16].cols[0], &row_zero.rows[17].cols[0], &row_zero.rows[18].cols[0], &row_zero.rows[19].cols[0],
    &row_zero.rows[20].cols[0], &row_zero.rows[21].cols[0], &row_zero.rows[22].cols[0], &row_zero.rows[23].cols[0],

    &row_one.rows[0].cols[0], &row_one.rows[1].cols[0], &row_one.rows[2].cols[0], &row_one.rows[3].cols[0],
    &row_one.rows[4].cols[0], &row_one.rows[5].cols[0], &row_one.rows[6].cols[0], &row_one.rows[7].cols[0],
    &row_one.rows[8].cols[0], &row_one.rows[9].cols[0], &row_one.rows[10].cols[0], &row_one.rows[11].cols[0],
    &row_one.rows[12].cols[0], &row_one.rows[13].cols[0], &row_one.rows[14].cols[0], &row_one.rows[15].cols[0],
    &row_one.rows[16].cols[0], &row_one.rows[17].cols[0], &row_one.rows[18].cols[0], &row_one.rows[19].cols[0],
    &row_one.rows[20].cols[0], &row_one.rows[21].cols[0], &row_one.rows[22].cols[0], &row_one.rows[23].cols[0],

    &row_two.rows[0].cols[0], &row_two.rows[1].cols[0], &row_two.rows[2].cols[0], &row_two.rows[3].cols[0],
    &row_two.rows[4].cols[0], &row_two.rows[5].cols[0], &row_two.rows[6].cols[0], &row_two.rows[7].cols[0],
    &row_two.rows[8].cols[0], &row_two.rows[9].cols[0], &row_two.rows[10].cols[0], &row_two.rows[11].cols[0],
    &row_two.rows[12].cols[0], &row_two.rows[13].cols[0], &row_two.rows[14].cols[0], &row_two.rows[15].cols[0],
    &row_two.rows[16].cols[0], &row_two.rows[17].cols[0], &row_two.rows[18].cols[0], &row_two.rows[19].cols[0],
    &row_two.rows[20].cols[0], &row_two.rows[21].cols[0], &row_two.rows[22].cols[0], &row_two.rows[23].cols[0],

    &row_three.rows[0].cols[0], &row_three.rows[1].cols[0], &row_three.rows[2].cols[0], &row_three.rows[3].cols[0],
    &row_three.rows[4].cols[0], &row_three.rows[5].cols[0], &row_three.rows[6].cols[0], &row_three.rows[7].cols[0],
    &row_three.rows[8].cols[0], &row_three.rows[9].cols[0], &row_three.rows[10].cols[0], &row_three.rows[11].cols[0],
    &row_three.rows[12].cols[0], &row_three.rows[13].cols[0], &row_three.rows[14].cols[0], &row_three.rows[15].cols[0],
    &row_three.rows[16].cols[0], &row_three.rows[17].cols[0], &row_three.rows[18].cols[0], &row_three.rows[19].cols[0],
    &row_three.rows[20].cols[0], &row_three.rows[21].cols[0], &row_three.rows[22].cols[0], &row_three.rows[23].cols[0]
};
