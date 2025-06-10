// (C) 2025 Brad Colbert

#ifndef __PLAYER_GRAPHICS_DATA_H__
#define __PLAYER_GRAPHICS_DATA_H__
#include <stdint.h>

typedef struct {
    uint8_t base_images[8][8];      // 64 bytes for 8 images, each 8x8 pixels
    uint8_t idle_animation[4][8];   // 32 bytes for 4 idle animation frames, each 8x8 pixels
    uint8_t other_animation[4][8];  // 32 bytes for 4 other animation frames, each 8x8 pixels
} PlayerImageData;

#endif