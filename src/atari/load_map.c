// (C) 2025 Brad Colbert

#include "playfield.h"

#include <conio.h>

#include <stdio.h>

// Defines
#define PLAYFIELD_TILE_ROWS 24
#define PLAYFIELD_ROWS (4 * PLAYFIELD_TILE_ROWS)
#define PLAYFIELD_COLS (4 * 40)

// Imports
extern tile_struct row_zero[];
extern tile_struct row_one[];
extern tile_struct row_two[];
extern tile_struct row_three[];

void load_map(void)
{
    int i, rows, cols, over;
    FILE* fp;

    fp = fopen("D:z2.atm", "rb");
    if(fp != NULL)
    {
        cprintf("\r                         \r%p\n\r", fp);

        rows = fgetc(fp);
        cols = fgetc(fp);
        cprintf("%d, %d\n\r", rows, cols);

        over = 0;
        for(i = 0; i < PLAYFIELD_TILE_ROWS; ++i)
        {
            over += fread(&row_zero[0].data[i][0], 1, PLAYFIELD_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_TILE_ROWS; ++i)
        {
            over += fread(&row_one[0].data[i][0], 1, PLAYFIELD_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_TILE_ROWS; ++i)
        {
            over += fread(&row_two[0].data[i][0], 1, PLAYFIELD_COLS, fp);
        }
        for(i = 0; i < PLAYFIELD_TILE_ROWS; ++i)
        {
            over += fread(&row_three[0].data[i][0], 1, PLAYFIELD_COLS, fp);
        }

        cprintf("Read %d bytes\n\r", over);
        cgetc();

        fclose(fp);
    }
    else
        cprintf("\nUnable to open map file      ");
}