// (C) 2025 Brad Colbert

// This file defines the critical data that contains the playfield and game data.
#include <stdint.h>

#include "playfield.h"

#pragma bss-name (push,"PLAYFIELD_ROW0")
tile_struct row_zero[4]; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma bss-name (push,"GAME_DATA0")
uint8_t buffer_zero[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW1")
tile_struct row_one[4]; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma bss-name (push,"GAME_DATA1")
uint8_t buffer_one[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW2")
tile_struct row_two[4]; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma data-name (push,"DISPLAYLIST")
// //uint8_t display_list[256]; // 16 bytes for the first buffer
#include "display_list_antic4.h"
//uint8_t buffer_two[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
#pragma data-name (pop)

#pragma data-name (push,"LOGO_DL")
#include "display_list_anticF.h"
#pragma data-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW3")
tile_struct row_three[4]; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)
