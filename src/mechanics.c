#include "mechanics.h"
#include "playfield.h"
#include "player_missiles.h"
#include "types.h"

#include <conio.h>

void update_playfield_using_players()
{
    // Find if the player is off the visible playfield, if so, move the playfield
    short p_x = players.one.x - PF_MIN_X;
    short p_y = players.one.y - PF_MIN_Y;
    if(p_x < vp_ul.x)
        set_playfield_viewport(p_x, vp_ul.y);
    if(p_x + PLAYER_WIDTH > vp_lr.x)
        set_playfield_viewport(p_x + PLAYER_WIDTH - PF_COLS_PER_PAGE, vp_ul.y);

    if(p_y < vp_ul.y)
        set_playfield_viewport(vp_ul.x, p_y);
    if(p_y + PLAYER_WIDTH > vp_lr.y)
        set_playfield_viewport(vp_ul.x, p_y + PLAYER_WIDTH - PF_LINES_PER_PAGE);

    #ifdef DEBUG_MECHANICS
    cprintf("\rpm    %d,%d\n", players.one.x, players.one.y);
    cprintf("\rpm sc %d,%d\n", p_x, p_y);
    cprintf("\rvp ul %d,%d\n", vp_ul.x, vp_ul.y);
    cprintf("\rvp lr %d,%d\n", vp_lr.x, vp_lr.y);
    gotoy(0);
    #endif
}
