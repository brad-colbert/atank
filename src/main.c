#include "mul16.h"
#include "shapes.h"
#include "map.h"
#include "drawline.h"
#include "graphics.h"
#include "line_clipping.h"

#include <cc65.h>
#include <conio.h>
#include <ctype.h>
//#include <tgi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TIME (OS.rtclok[1] * 256 + OS.rtclok[2])
#define TIME_RESET (OS.rtclok[1] = OS.rtclok[2] = 0)
#define SQUARE(x,y,w,h) { { x, y }, { x, y+h }, { x+w, y+h }, { x+w, y } }
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

extern uint8_t X1,Y1,X2,Y2;//XX15[4];
extern uint8_t XX13, SWAP;
extern Line lines[];

int main(void)
{
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

    #if 0
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 100;
    uint16_t y2 = 0;
    uint16_t idx = 0;
    #endif
    uint8_t idxb = 0;

    // Load the map
    uint8_t lineCount;
    loadMap("map.txt", &lineCount);

    printf("Line count: %d\n", lineCount);

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

    init_graphics();

    clear_graphics();

    cgetc();

    /* Do graphics stuff */
    TIME_RESET;

    #define USE_MAP
    #ifdef USE_MAP
    #else
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
}