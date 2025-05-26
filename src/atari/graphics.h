// (C) 2025 Brad Colbert

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "player.h"

void init_graphics();
void render_frame();
void shutdown_graphics();

void init_player_graphics(Player* player);
void update_player_graphics(Player* player);
void draw_player_graphics(Player* player);
void close_player_graphics(void);


#endif // __GRAPHICS_H__