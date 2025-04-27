#include "graphics.h"
#include "drawline.h"

#include <atari.h>

#include <stdint.h>
#include <string.h>

#define FRAMEBUFFER_SIZE 7696

extern uint8_t FBLUT_HI[];
extern uint8_t FBLUT_LO[];

#pragma optimize(push, off)

//#define USE_LOGO
#ifdef USE_LOGO
#pragma data-name (push,"FRAMEBUFFER")
#include "logo.h"
#pragma data-name (pop)
#else
#pragma bss-name (push,"FRAMEBUFFER")
uint8_t framebuffer[7680]; // 320x192 (/8)
#pragma bss-name (pop)
#endif

#pragma data-name (push,"GFX8_DL")
#include "graphics_8_dl.h"
#pragma data-name (pop)

#pragma optimize(pop)

// State save
void* ORG_SDLIST = 0;
void* VDSLIST_STATE = 0;
uint8_t ORG_GPRIOR = 0x0;
//uint8_t NMI_STATE = 0x0;
uint8_t ORG_COLOR1, ORG_COLOR2;

void init_graphics()
{
    uint8_t i;
    uint16_t addr;

    // Save the current graphics state
    ORG_SDLIST = OS.sdlst;
    VDSLIST_STATE = OS.vdslst;
    ORG_GPRIOR = OS.gprior;       // Save current priority states
    ORG_COLOR1 = OS.color1;
    ORG_COLOR2 = OS.color2;

    // Set the graphics color and mode
    OS.color1 = 14;         // Color maximum luminance
    OS.color2 = 0;          // Background black

    OS.sdlst = &graphics_8_dl;

    // Initilize the framebuffer address LUTs
    #if 0
    for(i = 0; i < 102; ++i) {
        addr = (uint16_t)&framebuffer[i * 40];
        FBLUT_HI[i] = (uint8_t)(addr>>8);
        FBLUT_LO[i] = (uint8_t)addr;
    }
    for(i = 0; i < 90; ++i) {
        addr = (uint16_t)&framebuffer[i * 40 + 4096];
        FBLUT_HI[i+102] = (uint8_t)(addr>>8);
        FBLUT_LO[i+102] = (uint8_t)addr;
    }
    #endif
    for(i = 0; i < 192; ++i) {
        addr = (uint16_t)&framebuffer[i * 40];
        FBLUT_HI[i] = (uint8_t)(addr>>8);
        FBLUT_LO[i] = (uint8_t)addr;
    }
}

void shutdown_graphics()
{
    OS.gprior = ORG_GPRIOR;           // Return original state of GTIA
    OS.sdlst = ORG_SDLIST;
    //ANTIC.nmien = NMIEN_VBI; //NMI_STATE;
    OS.vdslst = VDSLIST_STATE;
    //OS.botscr = 24;
    OS.color1 = ORG_COLOR1;
    OS.color2 = ORG_COLOR2;
}

void clear_graphics()
{
    memset((void*)framebuffer, 0, FRAMEBUFFER_SIZE);
}