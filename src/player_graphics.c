// Define for your platform.  This one is for Atari
#include "player_graphics.h"
//#include "standard_tank.h"
#include "player_graphics_data.h"

#include <atari.h>

#include <string.h> // for memset

// Externals

// Globals
#pragma bss-name (push,"PLAYERMISSILE")
PlayerMissiles pmg;
#pragma bss-name (pop)
#pragma data-name (push,"PLAYER_IMAGE_DATA")
extern PlayerGraphicsData player_image_data;
#pragma data-name (pop)
uint16_t SDMCTL_SAVE = 0x00;

void init_player_graphics(Player* player) {
    memset(&(pmg.player0[0]), 0x00, PLAYER_HEIGHT);
    memset(&(pmg.player1[0]), 0xAA, PLAYER_HEIGHT);
    memset(&(pmg.player2[0]), 0xAA, PLAYER_HEIGHT);
    memset(&(pmg.player3[0]), 0xAA, PLAYER_HEIGHT);

    #if 1
    ANTIC.pmbase = ((uint16_t)&pmg) >> 8;
    SDMCTL_SAVE = OS.sdmctl;
    OS.sdmctl = OS.sdmctl | DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_PLAYERS | DMACTL_DMA_MISSILES | DMACTL_DMA_FETCH | PLAYER_LINE_MODE;
    GTIA_WRITE.gractl = GRACTL_PLAYERS; //GRACTL_MISSLES
    OS.pcolr0 = 0x4A; //GTIA_COLOR_RED;
    OS.pcolr1 = 0x6A; //GTIA_COLOR_RED;
    OS.pcolr2 = 0xDA; //GTIA_COLOR_RED;
    OS.pcolr3 = 0x5A; //GTIA_COLOR_RED;
    //OS.pcolr1 = 0x3A;
    //OS.pcolr2 = 0x3A;
    //OS.pcolr3 = 0x3A;
    GTIA_WRITE.hposp0 = 0;
    GTIA_WRITE.hposp1 = 0;
    GTIA_WRITE.hposp2 = 0;
    GTIA_WRITE.hposp3 = 0;

    //OS.colpf1 = 0x00; //30;
    // GTIA_WRITE.colpf0 = 0x01;
//    GTIA_WRITE.colpf1 = 0x00; //30;
//    GTIA_WRITE.colpf2 = 0xEE;
    // GTIA_WRITE.colpf3 = 0x04;
//    GTIA_WRITE.colbk = 0x00;

    OS.gprior = 1; //PRIOR_PF03_P03;
    //GTIA_WRITE.prior = 1; //PRIOR_PF03_P03;
    #endif
}

void update_player_graphics(Player* player) {
    GTIA_WRITE.hposp0 = 127;   // horizontal position of player

    memcpy(&(pmg.player0[127-4]), player_image_data.player0.base_images[player->direction], 8);
}

void draw_player_graphics(Player* player) {}

void close_player_graphics() {
    memset(&pmg, 0x00, sizeof(PlayerMissiles));
    GTIA_WRITE.gractl = 0x00;
    OS.sdmctl = SDMCTL_SAVE;
}
