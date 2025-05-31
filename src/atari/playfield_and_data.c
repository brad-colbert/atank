// (C) 2025 Brad Colbert

// This file defines the critical data that contains the playfield, player, and game data.
#include <stdint.h>

#include "playfield.h"
#include "player_missiles.h"
#include "player_graphics_data.h"
#include "graphics.h"


//#pragma data-name (push,"FONT")
//#include "atari-small-4x8-COLOR1.h"
//#pragma data-name (pop)

#pragma bss-name (push,"PLAYERMISSILE")
PlayerMissiles player_missiles;
#pragma bss-name (pop)

// The gameplay display list is tucked into the empty 256 bytes in the player missile segment.
#pragma data-name (push,"DISPLAYLIST")
#include "display_list_antic4.h"
#pragma data-name (pop)

#pragma bss-name (push,"PLAYER_IMAGES0")
PlayerGraphics player0_graphics;
PlayerGraphics player1_graphics;
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYER_IMAGES1")
PlayerGraphics player2_graphics;
PlayerGraphics player3_graphics;
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW0")
playfield_block_struct row_zero; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma bss-name (push,"GAME_DATA0")
uint8_t buffer_zero[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW1")
playfield_block_struct row_one; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma bss-name (push,"GAME_DATA1")
uint8_t buffer_one[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
#pragma bss-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW2")
playfield_block_struct row_two; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)

#pragma bss-name (push,"GAME_DATA2")
//uint8_t buffer_two[256]; // Takes to 4096 bytes, on 4K boundary.  Easier math for display list.
graphics_state saved_graphics_state; // Save the graphics state for later restoration.
#pragma bss-name (pop)

// This will be clobbered when gameplay starts.
#pragma data-name (push,"LOGO_DL")
#include "display_list_anticF.h"
#pragma data-name (pop)

#pragma bss-name (push,"PLAYFIELD_ROW3")
playfield_block_struct row_three; // 3840 bytes per row.  Hows 4 screens worth of data.
#pragma bss-name (pop)
