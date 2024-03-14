// Our includes first
#include "mechanics.h"
#include "graphics.h"
#include "playfield.h"
#include "types.h"
#include "playfield_utils.h"
#include "player_missiles.h"

// Atari specific includes next
#include <atari.h>
#include <joystick.h>
#include <conio.h>

// Standard C includes
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define USE_PLAYERS_DRIVE_SCROLL
#define DELAY 64
#define JOY_3 2
#define JOY_4 3

byte joys[4];

void read_joysticks()
{
    switch(num_players)
    {
        case 4:
            joys[3] = joy_read(JOY_4);
        case 3:
            joys[2] = joy_read(JOY_3);
        case 2:
            joys[1] = joy_read(JOY_2);
        default:
            joys[0] = joy_read(JOY_1);
    }
}

int main()
{
    #ifdef USE_PLAYERS_DRIVE_SCROLL
    u_short delay;

    // Debug pause
    cprintf("\rHit Key\r");
    cgetc();  // Pause
    cprintf("         ");

    // Initialize the components
    init_playfield();
    init_graphics();
    init_player_missiles();
    joy_install(joy_static_stddrv);

    // Make sure the playfield is initialized
    set_playfield_viewport(0, 0);

    // Set the number of players (will be done on the opening screen)
    num_players = 1;

    // Initial joystick read
    read_joysticks();

    while(!JOY_BTN_1(joys[0]))
    {
        byte idx;
        for(idx = 0; idx < num_players; ++idx)
        {
            byte joy = joys[idx];
            if (JOY_UP(joys[idx])){
                if(players.all[idx].y > PF_MIN_Y)
                {
                    --players.all[idx].y;
                    players.all[idx].dirty = 1;
                }
            }
            else if (JOY_DOWN(joys[idx])){
                //if(players.all[idx].y < (PF_LINES - PF_LINES_PER_PAGE - (255-PF_MAX_Y))-1)
                if(players.all[idx].y < PF_LINES + 24) // + PF_LINES_PER_PAGE)
                {
                    ++players.all[idx].y;
                    players.all[idx].dirty = 1;
                }
            }
            if (JOY_LEFT(joys[idx])) {
                if(players.all[idx].x > PF_MIN_X)
                {
                    --players.all[idx].x;
                    players.all[idx].dirty = 1;
                }
            }
            else if (JOY_RIGHT(joys[idx])) {
                // *** Why 32!? **
                // On display list instructions with the horizontal scrolling bit set, ANTIC automatically expands
                // its screen memory use to the next larger playfield size, unless it is already using a wide playfield.
                // Scrolling with a 32 byte narrow playfield will cause ANTIC to read memory as if it were using a
                // normal 40 byte playfield, and scrolling a normal playfield will be processed as if
                // it were a wide 48 byte playfield.
                // https://playermissile.com/scrolling_tutorial/index.html#interlude-wide-and-narrow-playfields
                if(players.all[idx].x < (PF_COLS+28)) // + PF_COLS_PER_PAGE)
                {
                    ++players.all[idx].x;
                    players.all[idx].dirty = 1;
                }
            }
        }

        update_player_missiles();
        update_playfield_using_players();

        //for(delay = 0; delay < DELAY; ++delay);

        read_joysticks();
    }

    #endif //USE_PLAYERS_DRIVE_SCROLL

    #ifdef USE_JOYSTICK

    byte joy;
    u_short delay;
    u_short x, y;

    cprintf("Hit Key");
    cgetc();  // Pause

    init_graphics();
    init_playfield();
    init_player_missiles();

    num_players = 1;

    y = x = 0;

    joy_install(joy_static_stddrv);

    joy = joy_read(JOY_1);
    while (!JOY_BTN_1(joy))
    {
        if (JOY_UP(joy)){
            if(y > 0)
                --y;
        }
        else if (JOY_DOWN(joy)){
            if(y < (PF_LINES - PF_LINES_PER_PAGE)-1)
                ++y;
        }

        if (JOY_LEFT(joy)) {
            if(x > 0)
                --x;
        }
        else if (JOY_RIGHT(joy)) {
            if(x < (PF_COLS - PF_COLS_PER_PAGE)-1)
                ++x;
        }

        set_player_position(0, (byte)x, (byte)y);
        update_player_missiles();
        scroll_playfield((u_short)x, (u_short)y);

        for(delay = 0; delay < DELAY; ++delay);

        joy = joy_read(JOY_1);
    }

    joy_uninstall();

    #endif // USE_JOYSTICK

    #ifdef USE_BOUNCE_TEST
    u_short line_d, col_d;
    byte bounce_count = 0;

    u_short delay;
    u_short x, y;

    cprintf("Hit Key");
    cgetc();  // Pause

    init_graphics();
    init_playfield();

    line_d = col_d = 0;

    while (bounce_count < 100) // One hundred bounces
    {
        scroll_playfield(line, col);
        //cprintf("%d %d ", line, col);
        //cprintf("%d %d\n\r", line_d, col_d);

        // Update line and col
        line += line_d;
        col  += col_d;

        // Bounce
        if(line == (PF_LINES - PF_LINES_PER_PAGE)-1) {
            line_d = -1;
            ++bounce_count;
        }
        else if(line <= 0) {
            line_d = 1;
            ++bounce_count;
        }

        if(col == (PF_COLS - PF_COLS_PER_PAGE)-1) {
            col_d = -1;
            ++bounce_count;
        }
        else if(col <= 0) {
            col_d = 1;
            ++bounce_count;
        }
        for(delay = 0; delay < DELAY; ++delay);
        //sleep(1);
    }

    #endif // USE_BOUNCE_TEST

    close_player_missiles();
    close_graphics();

    cprintf("Hit Key To Close");
    cgetc();  // Pause

    return 0;
}
