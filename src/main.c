#include "mul16.h"
#include "shapes.h"
#include "map.h"
#include "drawline.h"
#include "graphics.h"

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

int main(void)
{
    uint16_t t_avg, count = 2;

    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 100;
    uint16_t y2 = 0;
    uint16_t idx = 0; //idx_y;

    // Load the map
    uint8_t lineCount;
    loadMap("map.txt", &lineCount);

    init_graphics();

    clear_graphics();

    cgetc();

    /* Do graphics stuff */
    TIME_RESET;

    // Vertical lines going across.
    x1 = 0;
    y1 = 1;
    x2 = 0;
    y2 = 190;
    
    for(idx = 0; idx < 320; idx+=10) {
        XORLineC(x1+idx, y1, x2+idx, y2);
        count++;
    }
    
    // Horizontal lines going down.
    x1 = 1;
    y1 = 0;
    x2 = 126;
    y2 = 0;

    for(idx = 0; idx < 192; ++idx) {
        XORLineC(x1+idx, y1+idx, x2+idx, y2+idx);
        count++;
    }

    t_avg = TIME/count;

    cgetc ();

    shutdown_graphics();

    cgetc ();

    cprintf("Time: %u\n", t_avg);

    cgetc();

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}