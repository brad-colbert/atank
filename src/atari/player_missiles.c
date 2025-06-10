// (C) 2025 Brad Colbert
#include "player_missiles.h"
#include "player_graphics_data.h"
#include "graphics.h"

#include <atari.h>
#include <string.h> // for memset
#include <stdint.h>

// Imports
extern graphics_state saved_graphics_state; // Defined in playfield_and_data.c
extern PlayerImageData player_0_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_1_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_2_image_data; // Defined in player_graphics_data.h
extern PlayerImageData player_3_image_data; // Defined in player_graphics_data.h
extern PlayerMissiles player_missiles;      // Defined in player_missiles.h

// Globals
uint8_t pm_x_coord_map[] = {
    47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 
    63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 
    79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 
    95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 
    127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 
    143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 
    159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 
    175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 
    191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 
    207
};
uint8_t pm_y_coord_map[] = {
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 224
};

// Call to setup player missiles
void init_player_missiles() {
    // Load player image data
    //memcpy(&(player_missiles.player0[127-4]), player_0_image_data.base_images[0], 8);
    memset(&(player_missiles.player0[0]), 0x00, PLAYER_HEIGHT);
    memset(&(player_missiles.player1[0]), 0x00, PLAYER_HEIGHT);
    memset(&(player_missiles.player2[0]), 0x00, PLAYER_HEIGHT);
    memset(&(player_missiles.player3[0]), 0x00, PLAYER_HEIGHT);

    // Set player missile base address
    saved_graphics_state.pmbase = ANTIC.pmbase;
    ANTIC.pmbase = ((uint16_t)&player_missiles) >> 8;

    // Enable player missiles in SDMCTL
    OS.sdmctl |= DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_PLAYERS | DMACTL_DMA_MISSILES | DMACTL_DMA_FETCH | PLAYER_LINE_MODE;

    // Set the graphics control register for player missiles
    GTIA_WRITE.gractl = GRACTL_MISSLES | GRACTL_PLAYERS;

    // Set player colors
    OS.pcolr0 = 0x4A; // Player 0 color
    OS.pcolr1 = 0x6A; // Player 1 color
    OS.pcolr2 = 0xDA; // Player 2 color
    OS.pcolr3 = 0x5A; // Player 3 color

    // Set initial horizontal positions for players
    GTIA_WRITE.hposp0 = 0;
    GTIA_WRITE.hposp1 = 0;
    GTIA_WRITE.hposp2 = 0;
    GTIA_WRITE.hposp3 = 0;

    OS.gprior = PRIOR_P03_PF03;
}

// Turn off player missiles
void disable_player_missiles() {
    // Clear player missile base address
    ANTIC.pmbase = saved_graphics_state.pmbase;

    // Disable player missiles
    GTIA_WRITE.gractl = 0x00;
    OS.sdmctl &= ~(DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_PLAYERS | DMACTL_DMA_MISSILES | DMACTL_DMA_FETCH | PLAYER_LINE_MODE);
    OS.gprior = 0x00; // Reset graphics priority
    OS.pcolr0 = 0x00; // Reset player-missile 0 color
    OS.pcolr1 = 0x00; // Reset player-missile 1 color
    OS.pcolr2 = 0x00; // Reset player-missile 2 color
    OS.pcolr3 = 0x00; // Reset player-missile 3 color
    GTIA_WRITE.hposp0 = 0; // Reset horizontal position of player 0
    GTIA_WRITE.hposp1 = 0; // Reset horizontal position of player 1
    GTIA_WRITE.hposp2 = 0; // Reset horizontal position of player 2
    GTIA_WRITE.hposp3 = 0; // Reset horizontal position of player 3

    // Wipe memory
    memset(&player_missiles.missile3210, 0x00, sizeof(PlayerMissiles)-768);
}