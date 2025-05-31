// (C) 2025 Brad Colbert

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "player.h"

// Types
typedef struct {
    void* sdlist;
    void* vdslst;
    uint8_t sdmctl; // ANTIC SDMCTL register
    uint8_t nmien;  // Non-maskable interrupt enable
    uint8_t gprior; // Graphics priority
    uint8_t pcolr0; // = $02C0          1-byte player-missile 0 color/luminance
    uint8_t pcolr1; // = $02C1          1-byte player-missile 1 color/luminance
    uint8_t pcolr2; // = $02C2          1-byte player-missile 2 color/luminance
    uint8_t pcolr3; // = $02C3          1-byte player-missile 3 color/luminance 
    uint8_t color0; // = $02C4          1-byte playfield 0 color/luminance
    uint8_t color1; // = $02C5          1-byte playfield 1 color/luminance
    uint8_t color2; // = $02C6          1-byte playfield 2 color/luminance
    uint8_t color3; // = $02C7          1-byte playfield 3 color/luminance
    uint8_t color4; // = $02C8          1-byte background color/luminance
    uint8_t chbas;  // = $02F?          1-byte character base address
    // 15 bytes total
    uint8_t buffer[256-17];
} graphics_state;

void init_graphics();
void render_frame();
void shutdown_graphics();

void init_player_graphics(Player* player);
void update_player_graphics(Player* player);
void draw_player_graphics(Player* player);
void close_player_graphics(void);

#endif // __GRAPHICS_H__