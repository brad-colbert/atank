// (C) 2025 Brad Colbert

#include "playfield.h"

#include <conio.h>

#include <stdio.h>

// Defines
// #define PLAYFIELD_TILE_ROWS 24
// #define PLAYFIELD_ROWS (4 * PLAYFIELD_TILE_ROWS)
// #define PLAYFIELD_COLS (4 * 40)
#define TILE_ROWS 4
#define TILE_COLS 4

// Imports
extern playfield_block_struct row_zero;
extern playfield_block_struct row_one;
extern playfield_block_struct row_two;
extern playfield_block_struct row_three;

void load_tile(uint8_t* tile, char* filename)
{
    FILE* fp;

    // Open the file
    fp = fopen(filename, "rb");

    if(fp != NULL)
    {
        // Read the tile data
        fread(tile, 1, 960, fp);
        fclose(fp);
    }
    else
    {
        cprintf("Unable to open map file %s\n\r", filename);
        return;
    }
}

void load_map(void)
{
    #if 0
    #define JUST_SHOW_FONT
    #ifdef JUST_SHOW_FONT
    uint8_t i, j;
    //uint8_t* here = (uint8_t*)&row_zero[0].data[0][0]; // Pointer to the first tile data

    // here[0] = 63;
    // here[1] = 1;
    // here[2] = 2;
    // here[3] = 3;
    // here[4] = 4;

    for(i = 0; i < 16; ++i)
    {
        for(j = 0; j < 16; ++j)
        {
            //here[(i * 40 * 4) + j] = (i*16) + j; // Fill row_zero with sequential numbers for testing
            row_zero.rows[i].cols[j] = (i*16) + j; // Fill row_zero with sequential numbers for testing
        }
    }
    #else
    char filename[15] = "D:tile00.scr";
    uint8_t i;

    for(i = 0; i < 1; ++i)
    {
        // Make the filename
        sprintf(filename, "D:TILE0%d.SCR", i);
        load_tile((uint8_t*)&row_zero.tiles[i], filename);

        // Make the filename
        // sprintf(filename, "D:TILE1%d.SCR", i);
        // load_tile((uint8_t*)&row_one[i], filename);

        // // Make the filename
        // sprintf(filename, "D:TILE2%d.SCR", i);
        // load_tile((uint8_t*)&row_two[i], filename);

        // // Make the filename
        // sprintf(filename, "D:TILE3%d.SCR", i);
        // load_tile((uint8_t*)&row_three[i], filename);
    }

    #endif

    #else

    int i, rows, cols, over;
    FILE* fp;

    //fp = fopen("D:z2.atm", "rb");
    fp = fopen("D:Z2.ATM", "rb");
    if(fp != NULL)
    {
        //cprintf("\r                         \r%p\n\r", fp);

        rows = fgetc(fp);
        cols = fgetc(fp);
        //cprintf("%d, %d\n\r", rows, cols);

        over = 0;
        for(i = 0; i < PLAYFIELD_ROWS; ++i)
        {
            over += fread(&row_zero.rows[i], 1, PLAYFIELD_COLS * PLAYFIELD_TILES_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_ROWS; ++i)
        {
            over += fread(&row_one.rows[i], 1, PLAYFIELD_COLS * PLAYFIELD_TILES_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_ROWS; ++i)
        {
            over += fread(&row_two.rows[i], 1, PLAYFIELD_COLS * PLAYFIELD_TILES_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_ROWS; ++i)
        {
            over += fread(&row_three.rows[i], 1, PLAYFIELD_COLS * PLAYFIELD_TILES_COLS, fp);
        }

        //cprintf("Read %d bytes\n\r", over);
        //cgetc();

        fclose(fp);
    }
    else
        cprintf("\nUnable to open map file      ");
    #endif
}