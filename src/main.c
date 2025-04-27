#include "mul16.h"
#include "shapes.h"
#include "map.h"
#include "drawline.h"
#include "graphics.h"
#include "line_clipping.h"
#include "arith16_coord_array.h"

#include <cc65.h>
#include <conio.h>
#include <ctype.h>
// #include <tgi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TIME (OS.rtclok[1] * 256 + OS.rtclok[2])
#define TIME_RESET (OS.rtclok[1] = OS.rtclok[2] = 0)
#define SQUARE(x, y, w, h)                               \
    {                                                    \
        {x, y}, {x, y + h}, {x + w, y + h}, { x + w, y } \
    }
#define WIDTH 320

/*****************************************************************************/
/*                                   Type                                    */
/*****************************************************************************/

/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/

extern uint8_t framebuffer[7696]; // Assume framebuffer is defined elsewhere

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

extern uint8_t X1, Y1, X2, Y2; // XX15[4];
extern uint8_t XX13, SWAP;
extern Line lines[];
extern int16_t *base_ptr;
extern uint8_t coord_count;
extern int16_t X_val;
extern int16_t Y_val;

extern uint8_t CARRY;  // Global or static variable to store carry flag state

uint8_t coords[2][32][4];
Point pos = { 1, 1 }; //{ 320/2, 192/2 }; // Position of the player

int main(void)
{

#define TEST_LOAD_MAP
#ifdef TEST_LOAD_MAP

#if 0
    //   x1 y1 x2 y2
    clip(10,0,100,200);

    printf("\nx1: %d\n", XX15[0]);
    printf("y1: %d\n", XX15[1]);
    printf("x2: %d\n", XX15[2]);
    printf("y2: %d\n", XX15[3]);

    cgetc();

#else

    uint8_t active_buff = 0;
    uint16_t t_avg, count = 2;

#if 0
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 100;
    uint16_t y2 = 0;
    uint16_t idx = 0;
#endif
    uint8_t idxb;
    uint8_t x, y;
    uint8_t lineCount;            // Number of lines in the map

    // Load the map
    loadMap("map.txt", &lineCount);
    printf("Line count: %d\n", lineCount);

    // Set the arguments to the coordinate translation function
    base_ptr = (int16_t*)&lines[0];
    coord_count = lineCount*2;       // Number of coordinates to translate

    #define TEST_TRANSLATE_MAP_LINESX
    #ifdef TEST_TRANSLATE_MAP_LINES
    X_val = 0;
    Y_val = 0;

    // Translate the coordinates
    arith16_coord_array();

    for(idxb = 0; idxb < lineCount; ++idxb) {
        if((idxb % 9) == 0) {
            cprintf("Press any key to continue...\n\r");
            cgetc();
        }
        cprintf("Line %d:     %4d %4d %4d %4d\n\r", idxb, lines[idxb].start.x, lines[idxb].start.y, lines[idxb].end.x, lines[idxb].end.y);
        clip(lines[idxb].start.x, lines[idxb].start.y, lines[idxb].end.x, lines[idxb].end.y);
        test_carry_and_store();
        if(CARRY == 0) {
            if(SWAP)
                cprintf("Cl*p %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, X2, Y2, X1, Y1);
            else
                cprintf("Clip %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, X1, Y1, X2, Y2);
        } else {
            cprintf("Not visible\n\r");
        }
    }
    cgetc();
    #endif

    init_graphics();
    clear_graphics();

    #define TEST_TRANSLATE_CLIP_DRAW
    #ifdef TEST_TRANSLATE_CLIP_DRAW
    #if 0
    // Vertical line, right of which is the status bar
    XORLineC(256, 0, 256, 191);
    XORLineC(257, 0, 319, 0);
    XORLineC(319, 1, 319, 191);
    XORLineC(257, 191, 318, 191);
    #endif

    // Translate the coordinates
    X1 = 93;
    X2 = 93;
    Y1 = 1;
    Y2 = 191;
    while(!kbhit()) {
        draw_line();
        //cgetc();
        //draw_line(); // erase
        ++X1;
        ++X2;
        #if 0
        X_val = -pos.x;
        Y_val = -pos.y;
    
        // Translate the coordinates
        //base_ptr = (int16_t*)&lines[0];
        arith16_coord_array();

        // Clip and draw the lines
        for(idxb = 0; idxb < lineCount; ++idxb) {
            clip(lines[idxb].start.x, lines[idxb].start.y, lines[idxb].end.x, lines[idxb].end.y);

            test_carry_and_store();
            if(CARRY == 0) {
                if(SWAP) {
                    x1 = X2;
                    y1 = Y2;
                    x2 = X1;
                    y2 = Y1;

                    // Store the coords, so we can re-draw next time to erase
                    coords[active_buff][idxb][0] = X2;
                    coords[active_buff][idxb][1] = Y2;
                    coords[active_buff][idxb][2] = X1;
                    coords[active_buff][idxb][3] = Y1;
                } else {
                    x1 = X1;
                    y1 = Y1;
                    x2 = X2;
                    y2 = Y2;

                    // Store the coords, so we can re-draw next time to erase
                    coords[active_buff][idxb][0] = X1;
                    coords[active_buff][idxb][1] = Y1;
                    coords[active_buff][idxb][2] = X2;
                    coords[active_buff][idxb][3] = Y2;
                }

                // Draw the line
                XORLineC(x1, y1, x2, y2);
            }
        }

        // Change to the other buffer.  Redraw to erase the old lines
        active_buff ^= 1;
        for(idxb = 0; idxb < lineCount; ++idxb) {
            x1 = coords[active_buff][idxb][0];
            y1 = coords[active_buff][idxb][1];
            x2 = coords[active_buff][idxb][2];
            y2 = coords[active_buff][idxb][3];

            // Draw the line
            XORLineC(x1, y1, x2, y2);
        }

        // Update the position
        if(X_val < -50) {
            pos.x = -1;
        }
        else if(X_val > 50) {
            pos.x = 1;
        }
        if(Y_val < -30) {
            pos.y = 1;
        }
        else if(Y_val > 30) {
            pos.y = -1;
        }
        #endif
    }
    cgetc();

    #endif

    /* Do graphics stuff */
    TIME_RESET;

#define TEST_16_BIT_LINE_COORD_DRAWX
#ifdef TEST_16_BIT_LINE_COORD_DRAW
    #define DRAW_VERTICAL_LINESX
    #ifdef DRAW_VERTICAL_LINES
    // Vertical lines going across.
    x1 = 0;
    y1 = 1;
    x2 = 0;
    y2 = 190;
    
    for(idx = 0; idx < 320; idx+=10) {
        XORLine_16(x1+idx, y1, x2+idx, y2);
        count++;
    }
    #else
    // Horizontal lines going down.
    x1 = 1;
    y1 = 0;
    x2 = 126;
    y2 = 0;

    for(idx = 0; idx < 192; ++idx) {
        XORLineC(x1+idx, y1+idx, x2+idx, y2+idx);
        count++;
    }
    #endif
#endif

    t_avg = TIME/count;

    cgetc ();

    shutdown_graphics();

    cgetc ();

    cprintf("Time: %u\n", t_avg);

    cgetc();

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
#endif

#endif

#ifdef TEST_ARITH16_COORD_ARRAY
    int idx = 0;
    int16_t a[] = { 3, 30, 300, 3000 };

    // Tell where the values to add are
    base_ptr = a;
    X_val = 123;
    Y_val = -10;

    // Tell the function how many coordinates we have
    __asm__("ldx #%b", 2);

    arith16_coord_array();

    for (; idx < 4; ++idx)
        printf("%d ", a[idx]);

    cgetc();
#endif
}