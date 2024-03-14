#include "graphics.h"
#include "types.h"

#pragma data-name (push,"DISPLAYLIST")
#include "dl.h"
#pragma data-name (pop)

#include <atari.h>
#include <conio.h>

byte CHBASE_OG;
byte SDMCTL_PREV;
short SDLST_PREV;
byte* DL;

//#define LOAD_FONT
#ifdef LOAD_FONT
#pragma data-name (push,"MYFONT")
#include "atari-small-4x8-COLOR2.h"
#pragma data-name (pop)
#else
extern char* font;
#endif

void init_font()
{
    CHBASE_OG = OS.chbas;

    //#define COPY_BASE_FONT
    #ifdef COPY_BASE_FONT
    u_short i;
    for(i = 0; i < 1024; ++i) {
        font[i] = *((byte*)0xE000 + i);
    }
    #endif

    // Use the new charset
    OS.chbas = (byte)((short)&font >> 8);
}

void init_graphics()
{
    // Save the previous ANTIC state
    SDMCTL_PREV = OS.sdmctl;
    SDLST_PREV = (short)OS.sdlst;

    // Turn off ANTIC
    OS.sdmctl = 0;

    // Set our mode
    #ifdef GFX7
    DL = (byte*)(&DISPLAY_LIST_GFX7);
    #else
    DL = (byte*)(&DISPLAY_LIST_ANTIC4);
    #endif

    // Enable the display list
    #ifdef GFX7
    OS.sdlst = &DISPLAY_LIST_GFX7;
    #else
    OS.sdlst = &DISPLAY_LIST_ANTIC4;
    #endif

    // Install our charset
    init_font();

    // Turn On ANTIC
    OS.sdmctl = SDMCTL_PREV;
}

void close_graphics()
{
    // Disable ANTIC
    OS.sdmctl = 0;

    // Restore the original display list
    OS.sdlst = (void*)SDLST_PREV;

    // Restore the standard character set
    OS.chbas = CHBASE_OG;

    // Turn On ANTIC
    OS.sdmctl = SDMCTL_PREV;
}