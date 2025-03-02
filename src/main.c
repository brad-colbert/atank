#if 0
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
                if(players.all[idx].y < PF_LINES + PLAYFIELD_PLAYER_EDGE_Y) // + PF_LINES_PER_PAGE)
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
                if(players.all[idx].x < (PF_COLS+PLAYFIELD_PLAYER_EDGE_X)) // + PF_COLS_PER_PAGE)
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
#else
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <tgi.h>



#if 0
#ifndef DYN_DRV
#  define DYN_DRV       1
#endif
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_FORE      TGI_COLOR_WHITE


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Driver stuff */
static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void CheckError (const char* S)
{
    unsigned char Error = tgi_geterror ();

    if (Error != TGI_ERR_OK) {
        printf ("%s: %u: %s\n", S, Error, tgi_geterrormsg(Error));
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}



#if DYN_DRV
static void DoWarning (void)
/* Warn the user that the dynamic TGI driver is needed for this program */
{
    printf ("Warning: This program needs the TGI\n"
            "driver on disk! Press 'y' if you have\n"
            "it - any other key exits.\n");
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    printf ("OK. Please wait patiently...\n");
}
#endif



static void DoCircles (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLUE };
    unsigned char I;
    unsigned char Color = COLOR_BACK;
    const unsigned X = MaxX / 2;
    const unsigned Y = MaxY / 2;
    const unsigned Limit = (X < Y) ? Y : X;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();
    tgi_line (0, 0, MaxX, MaxY);
    tgi_line (0, MaxY, MaxX, 0);
    while (!kbhit ()) {
        Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
        tgi_setcolor (Color);
        for (I = 10; I <= Limit; I += 10) {
            tgi_ellipse (X, Y, I, tgi_imulround (I, AspectRatio));
        }
    }

    cgetc ();
}



static void DoCheckerboard (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    unsigned X, Y;
    unsigned char Color = COLOR_BACK;

    tgi_setpalette (Palette);
    tgi_clear ();

    while (1) {
        for (Y = 0; Y <= MaxY; Y += 10) {
            for (X = 0; X <= MaxX; X += 10) {
                Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
                tgi_setcolor (Color);
                tgi_bar (X, Y, X+9, Y+9);
                if (kbhit ()) {
                    cgetc ();
                    return;
                }
            }
            Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
        }
        Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
    }
}



static void DoDiagram (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    int XOrigin, YOrigin;
    int Amp;
    int X, Y;
    unsigned I;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    /* Determine zero and amplitude */
    YOrigin = MaxY / 2;
    XOrigin = 10;
    Amp     = (MaxY - 19) / 2;

    /* Y axis */
    tgi_line (XOrigin, 10, XOrigin, MaxY-10);
    tgi_line (XOrigin-2, 12, XOrigin, 10);
    tgi_lineto (XOrigin+2, 12);

    /* X axis */
    tgi_line (XOrigin, YOrigin, MaxX-10, YOrigin);
    tgi_line (MaxX-12, YOrigin-2, MaxX-10, YOrigin);
    tgi_lineto (MaxX-12, YOrigin+2);

    /* Sine */
    tgi_gotoxy (XOrigin, YOrigin);
    for (I = 0; I <= 360; I += 5) {

        /* Calculate the next points */
        X = (int) (((long) (MaxX - 19) * I) / 360);
        Y = (int) (((long) Amp * -_sin (I)) / 256);

        /* Draw the line */
        tgi_lineto (XOrigin + X, YOrigin + Y);
    }

    cgetc ();
}



static void DoLines (void)
{
    //static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    static const unsigned char Palette[4] = { TGI_COLOR_BLACK, TGI_COLOR_WHITE, TGI_COLOR_RED, TGI_COLOR_GREEN };

    unsigned X;
    const unsigned Min = (MaxX < MaxY) ? MaxX : MaxY;

    tgi_setpalette (Palette);
    tgi_setcolor (2);
    tgi_clear ();

    /**/
    for (X = 0; X <= Min; X += 10) {
        tgi_line (0, 0, Min, X);
        tgi_line (0, 0, X, Min);
        tgi_line (Min, Min, 0, (Min-X));
        tgi_line (Min, Min, Min-X, 0);
    }
    /**/


    //cgetc ();
}



int main (void)
{
    unsigned char Border;

#if DYN_DRV
    /* Warn the user that the tgi driver is needed */
    DoWarning ();

    /* Load and initialize the driver */
    tgi_load_driver (tgi_stddrv);
    CheckError ("tgi_load_driver");
#else
    /* Install the driver */
    //tgi_install (atr15p2_tgi); //tgi_static_stddrv); //
    tgi_install (atr8p2_tgi); //tgi_static_stddrv); //
    CheckError ("tgi_install");
#endif

    tgi_init ();
    CheckError ("tgi_init");

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

    /* Set the palette, set the border color */
    Border = bordercolor (COLOR_BLACK);

    /* Do graphics stuff */
    tgi_setviewpage(0); // Show page 1
    tgi_setdrawpage(1);
    //DoCircles ();
    //DoCheckerboard ();
    //DoDiagram ();
    DoLines ();
    tgi_setviewpage(1); // Show page 1

    cgetc ();

#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif

    /* Reset the border */
    (void) bordercolor (Border);

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}
#endif