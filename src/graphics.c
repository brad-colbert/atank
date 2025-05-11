#include "graphics.h"

#include <atari.h>

#include <stdint.h>
#include <string.h>  // for memset

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
uint8_t framebuffer[FRAMEBUFFER_SIZE];
#pragma bss-name (pop)
#endif

#pragma data-name (push,"GFX8_DL")
#include "graphics_8_dl.h"
#pragma data-name (pop)

#pragma bss-name (push,"FBLUTLO")
uint8_t FBLUT_LO[HEIGHT_PIXELS];
#pragma bss-name (pop)

#pragma bss-name (push,"FBLUTHI")
uint8_t FBLUT_HI[HEIGHT_PIXELS];
#pragma bss-name (pop)

#pragma optimize(pop)

// Startup state
void* ORG_SDLIST = 0;
void* VDSLIST_STATE = 0;
uint8_t ORG_GPRIOR = 0x0;
uint8_t ORG_COLOR1, ORG_COLOR2;

// Configures the graphics mode and framebuffer
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

    // Initilize the framebuffer address LUTs.  This is used for faster line lookups.
    for(i = 0; i < HEIGHT_PIXELS; ++i) {
        addr = (uint16_t)&framebuffer[i * WIDTH_BYTES];
        FBLUT_HI[i] = (uint8_t)(addr>>8);
        FBLUT_LO[i] = (uint8_t)addr;
    }
}

void shutdown_graphics()
{
    OS.gprior = ORG_GPRIOR;           // Return original state of GTIA
    OS.sdlst = ORG_SDLIST;
    OS.vdslst = VDSLIST_STATE;
    OS.color1 = ORG_COLOR1;
    OS.color2 = ORG_COLOR2;
}

void clear_graphics()
{
    memset((void*)framebuffer, 0, FRAMEBUFFER_SIZE);
}