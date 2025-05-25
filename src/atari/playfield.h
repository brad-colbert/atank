// (C) 2025 Brad Colbert
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include <stdint.h>

typedef struct tile_s {
    uint8_t data[25][40]; // 1000 bytes per tile
} tile_struct;

typedef struct playfield_s {
    uint8_t data_one[4080]; // 4080 bytes for the first part (102 lines of 40 bytes each)
    uint8_t buffer_one[16]; // 16 bytes for the first buffer
    uint8_t data_two[4080]; // 4080 bytes for the second part
    uint8_t buffer_two[16]; // 16 bytes for the second buffer
    uint8_t data_three[4080]; // 4080 bytes for the third part
    uint8_t buffer_three[16]; // 16 bytes for the third buffer
    uint8_t data_four[4080]; // 4080 bytes for the fourth part
    
} playfield_details_struct;  // total 1600

{
    /* data */
};

uint8_t playfield

#endif // __PLAYFIELD_H__