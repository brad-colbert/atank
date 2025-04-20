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

    uint16_t t_avg, count = 2;

#if 1
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 100;
    uint16_t y2 = 0;
    uint16_t idx = 0;
#endif
    uint8_t idxb;
    uint8_t x, y;
    uint8_t lineCount;            // Number of lines in the map
    Point pos = { 320/2, 192/2 }; // Position of the player

    // Load the map
    loadMap("map.txt", &lineCount);
    printf("Line count: %d\n", lineCount);

    // Set the arguments to the coordinate translation function
    base_ptr = (int16_t*)&lines[0];
    coord_count = lineCount*2;       // Number of coordinates to translate

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
        if(SWAP)
            //cprintf("Clip %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, XX15[2], XX15[3], XX15[0], XX15[1]);
            cprintf("Clip %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, X2, Y2, X1, Y1);
        else
            //cprintf("Clip %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, XX15[0], XX15[1], XX15[2], XX15[3]);
            cprintf("Clip %d: %3d|%4d %4d %4d %4d\n\r", idxb, XX13, X1, Y1, X2, Y2);
    }
    cgetc();
    #endif

    init_graphics();
    clear_graphics();

    #ifdef TEST_TRANLATE_CLIP_DRAW
    // Translate the coordinates
    for(y = 0; y < 192; ++y) {
        //for(x = 0; x < 320; ++x) {
            X_val = -y;
            Y_val = -y;
        
            // Translate the coordinates
            arith16_coord_array();

            // Clip and draw the lines
            for(idxb = 0; idxb < lineCount; ++idxb) {
                clip(lines[idxb].start.x, lines[idxb].start.y, lines[idxb].end.x, lines[idxb].end.y);
                drawLineB(X1, Y1, X2, Y2);
            }
        //}
    }

    cgetc();
    #endif

    /* Do graphics stuff */
    TIME_RESET;

#define TEST_16_BIT_LINE_COORD_DRAW
#ifdef TEST_16_BIT_LINE_COORD_DRAW
    // Vertical lines going across.
    // x1 = 0;
    // y1 = 1;
    // x2 = 0;
    // y2 = 190;
    
    // for(idx = 0; idx < 320; idx+=10) {
    //     XORLineC(x1+idx, y1, x2+idx, y2);
    //     count++;
    // }
    
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