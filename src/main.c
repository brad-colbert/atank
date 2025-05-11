#define DEBUG

#include "map.h"
#include "graphics.h"

#include <stdio.h>
#include <stdint.h>

// Externals
extern uint8_t line_count;
extern Line lines[];
extern Point map_center;

// Globals

void main(void)
{
    // Load the map
    load_map("map.txt", &line_count);

    #ifdef DEBUG
    fprintf(stderr, "Line count: %d\n", line_count);
    #endif

    // Initialize graphics
    init_graphics();

    // Clear the screen
    clear_graphics();


    while(1) {}
}