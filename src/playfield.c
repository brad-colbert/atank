#include "playfield.h"
#include "graphics.h"
#include "types.h"
#include "playfield_utils.h"

// Atari specific includes next
#include <atari.h>
#include <conio.h>
#include <peekpoke.h>

// Standard C includes
#include <stdio.h>
#include <string.h>

#define COLOR_CLOCK_BUFF 4

// A description about the "playfield":
// There are PF_PAGE_ROWS x PF_PAGE_COLS pages in the playfield.
// Each page consists of PF_ROW_TILES x PF_COL_TILES tiles.
// Each tile is PF_ROW_PIX x PF_COL_PIX in pixels.
// A line is a pixel in row space, and a col (maybe horrible name) is a pixel in col space.

#pragma data-name(push,"ZEROPAGE")
byte scroll_flag = 0x00; // goes into ZEROPAGE
byte col = 0x00;         // course scroll playfield row byte offset
byte row = 0x00;         // course scroll playfield address LUT index
byte h_fs = 0x00;        // horizontal fine scroll
byte v_fs = 0x00;        // vertical fine scroll
#pragma data-name(pop)

//#define DEFINE_PF_LOCALY
#ifdef DEFINE_PF_LOCALY
#pragma data-name (push,"PLAYFIELD")
byte playfield[PF_COURSE_ROWS + NUM_4K_OVERLAPS][PF_COURSE_COLS];
#pragma data-name (pop)
#else
#include "playfield_1.h"
#endif
byte* address_lut[PF_COURSE_ROWS];
coord vp_ul = {0,0};
coord vp_lr = {PF_COLS_PER_PAGE, PF_LINES_PER_PAGE};

extern void DISPLAY_LIST_ANTIC4;

// Convert an address to text hex and stores those chars
// add a destination address.
void addr_to_hex_to_addr(byte* src, byte* dst)
{
    //char lut[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char lut[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 33, 34, 35, 36, 37, 38};
    u_short src_s = (u_short)src;
    byte ival = 0x00;
    byte value = 0x00;
    #define OFFSET 0

    // First char
    ival = (byte)(src_s >> 8);
    value = (ival & 0xF0) >> 4;
    *(dst + OFFSET + 0) = lut[value];

    // Second char
    value = ival & 0x0F;
    *(dst + OFFSET + 1) = lut[value];

    // Third char
    ival = (byte)(src_s & 0x00FF);
    value = (ival & 0xF0) >> 4;
    *(dst + OFFSET + 2) = lut[value];

    // Fourth char
    value = ival & 0x0F;
    *(dst + OFFSET + 3) = lut[value];
}

void init_playfield()
{
    int i;
    byte rows, cols;
    u_short next_addr, over;
    #define LOAD_MAP
    #ifdef LOAD_MAP
    FILE* fp;
    #endif

    // Fill the address LUT used for faster scrolling
    // Initialize the address LUT
    rows = 0;
    next_addr = (u_short)playfield;
    for(i = 0; i < PF_COURSE_ROWS; ++i)
    {
        address_lut[i] = next_addr;

        next_addr = (u_short)(address_lut[i]) + (u_short)PF_COURSE_COLS;
        over = (next_addr + (u_short)PF_COURSE_COLS) % (u_short)0x1000;  // can't cross 4K boundary
        if(over < (u_short)PF_COURSE_COLS)
        {
            next_addr = (next_addr & (u_short)0xF000) + (u_short)0x1000;
            ++row;  // count the number of 4K overlaps
        }
    }
    cprintf("Row overlaps: %d\n\r", row);
    cgetc();
    // initialize addresses in the display list
    for(i = 0; i < PF_ROW_TILES; ++i)
    {
        POKEW(((byte*)&DISPLAY_LIST_ANTIC4) + 4 + i * 3, address_lut[i]);
    }

    //#define GEN_TEST_MAP
    #ifdef GEN_TEST_MAP
    for(i = 1; i < PF_COURSE_ROWS; ++i)
    {
        u_short addr = (u_short)(address_lut[i-1]) + (u_short)PF_COURSE_COLS;
        u_short addr_next = (u_short)addr + (u_short)PF_COURSE_COLS;

        if(addr_next % 0x1000 < (u_short)PF_COURSE_COLS)
            addr = addr_next & 0xFF00;

        address_lut[i] = (byte*)addr;
    }
    #endif

    // Load the first map
    #ifdef LOAD_MAP
    fp = fopen("D:z2.srm", "rb");
    if(fp != NULL)
    {
        cprintf("\r                         \r%p\n\r", fp);

        rows = fgetc(fp);
        cols = fgetc(fp);
        cprintf("%d, %d\n\r", rows, cols);

        over = 0;
        for(i = 0; i < PF_COURSE_ROWS; ++i)
        {
            over += fread(address_lut[i], 1, PF_COURSE_COLS, fp);
        }

        cprintf("Read %d bytes\n\r", over);
        cgetc();

        fclose(fp);
    }
    else
        cprintf("\nUnable to open map file      ");
    #else
    for(row = 0; row < PF_COURSE_ROWS; ++row)
    {
        for(col = 0; col < PF_COURSE_COLS; col+=40)
        {
            byte* addr = address_lut[row] + col;
            addr_to_hex_to_addr(addr, addr);
        }
    }
    #endif

    setup_vbi();
}

// X and Y specify the upper left hand corner (ULC) of the viewable
// portion (viewport) of the playfield.  These are in pixel space which
// is implemented using fine scrolling to move to the specific
// upper left position.
void set_playfield_viewport(u_short x, u_short y)
{
    byte h_fs_lcl, v_fs_lcl, col_lcl, row_lcl;

    // Update the current coords for the viewport
    vp_ul.x = x;
    vp_ul.y = y;
    vp_lr.x = vp_ul.x + PF_COLS_PER_PAGE;
    vp_lr.y = vp_ul.y + PF_LINES_PER_PAGE;

    // Course scroll coordinates are used to select the byte level
    // memory location of the playfield.
    col_lcl = (byte)(x  / (u_short)PF_COL_PIX);
    row_lcl = (byte)(y / (u_short)PF_ROW_PIX);

    // Fine scroll coordinates
    h_fs_lcl = PF_COL_PIX - (byte)(x %  (u_short)PF_COL_PIX);
    v_fs_lcl = (byte)(y % (u_short)PF_ROW_PIX); 

    // Update the memory locations used by the VBI so it can update
    // the display lists (for course scroll) and fine scroll memory locations.
    // Keep the VBI from reading the coordinates while we write them.
    scroll_flag = 0xFF;

    col = col_lcl;
    row = row_lcl;

    h_fs = h_fs_lcl;
    v_fs = v_fs_lcl; 

    scroll_flag = 0x00;  // Allow the VBI to update the DL and scroll var's
}
