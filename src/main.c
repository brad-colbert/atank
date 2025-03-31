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

    const uint16_t x1 = 0;
    const uint16_t y1 = 0;
    const uint16_t x2 = 100;
    const uint16_t y2 = 0;
    uint16_t idx_x = 0; //idx_y;
    uint16_t offset;
    uint16_t address;
    //uint8_t b;

    // Load the map
    uint8_t lineCount;
    loadMap("map.txt", &lineCount);

    //cgetc();

    init_graphics();
    //cgetc();
    clear_graphics();

    /* Do graphics stuff */
    TIME_RESET;
    #if 0
    // tgi_setcolor(COLOR_WHITE);
    //while(!kbhit()) {
    //    ++count;
    for(; count < 320; ++count) {

        setPixel(framebuffer + 0x1000); //, 0);

        // Check for horizontal line
        if (y1 == y2) {
            for (idx_y = x1; idx_y <= x2; ++idx_y) {
                //offset = idx_y * WIDTH + (uint16_t)10;
                //if(idx_y < 102)
                //    address = &(framebuffer[offset/8]);
                //else
                //    address = &(framebuffer[(offset/8)+16]);
                // XOR the pixel at (x, y1)
                // Assuming a function setPixel(x, y) that sets the pixel at (x, y)
                //printf("h:%d,%d,%d,%d\n", idx_y, offset, offset/8, offset%8);
                //setPixel(address); //, (uint8_t)(offset%8)); // Replace with actual pixel manipulation code

                setPixelXY(count, (uint8_t)idx_y); // Replace with actual pixel manipulation code
            }
        }
    
        //cgetc();
    }
    #else
    cgetc();
    //b = (uint8_t)(x1 % 8); // Get the bit position of the first pixel
    for(idx_x = 0; idx_x < 192; ++idx_x) {
        //for(count = 0; count < 8; ++count) {
            XORLineC(x1+idx_x, count+idx_x, x2+idx_x, count+idx_x);
            #if 0
            // Write the left most pixel
            setPixelXYmask((uint8_t)(x1/8), (uint8_t)y1, b+(b-1));
            for(idx_y = (x1/8)+1; idx_y < (x2/8); ++idx_y) {
                setPixelXYmask((uint8_t)idx_y, (uint8_t)count, 0xFF); // Replace with actual pixel manipulation code
            }
            // Write the right most pixel FF-(2^bit-1)
            setPixelXYmask((uint8_t)(x2/8), (uint8_t)y1, 0xFF-(b-1));
            #endif
        //}
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
}