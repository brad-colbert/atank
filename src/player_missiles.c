#include "playfield.h"
#include "player_missiles.h"
#include "player_1.h"
#include "types.h"

#include <atari.h>
#include <conio.h>

#include <string.h>

#pragma bss-name (push,"PLAYERMISSILE")
PlayerMissiles player_missiles;
#pragma bss-name (pop)

#pragma data-name(push,"ZEROPAGE")
byte idx;
byte num_players = 0;
#pragma data-name(pop)

Players players;
u_short SDMCTL_SAVE;

void init_player_missiles()
{
    memset(&player_missiles, 0X00, sizeof(PlayerMissiles));

    ANTIC.pmbase = ((u_short)&player_missiles) >> 8;
    SDMCTL_SAVE = OS.sdmctl;
    OS.sdmctl = OS.sdmctl | DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_PLAYERS | DMACTL_DMA_MISSILES | DMACTL_DMA_FETCH | PLAYER_LINE_MODE;
    GTIA_WRITE.gractl = 0x03;
    OS.pcolr0 = GTIA_COLOR_LIGHTRED; //0x3A;  // Red
    OS.pcolr1 = 0x3A;  // Red
    OS.pcolr2 = 0x3A;  // Red
    OS.pcolr3 = 0x3A;  // Red
    GTIA_WRITE.hposp0 = 0;
    GTIA_WRITE.hposp1 = 0;
    GTIA_WRITE.hposp2 = 0;
    GTIA_WRITE.hposp3 = 0;
    OS.paddl0 = 1;             // PMs in front of all

    OS.rtclok[0] = OS.rtclok[1] = OS.rtclok[2] = 0x00;

    for(idx = 0; idx < MAX_PLAYERS; ++idx)
    {
        players.all[idx].dirty = 0;      // True when state changes and must be updated.
        players.all[idx].x = PF_MIN_X;
        players.all[idx].y = PF_MIN_Y;
        players.all[idx].image_idx = 0;
        players.all[idx].anim_dir = 1;
    }
}

void close_player_missiles(void)
{
    memset(&player_missiles, 0X00, sizeof(PlayerMissiles));
    GTIA_WRITE.gractl = 0x00;
    OS.sdmctl = SDMCTL_SAVE;
}

void set_player_position(byte num, byte x, byte y)
{
    Player* player = &players.one + sizeof(Player) * (u_short)num;

    player->x = x;
    player->y = y;
    player->dirty = 1;      // True when state changes and must be updated.
}

void update_player_missiles()
{
    Player* player;

    // Quick check on collision
    if(GTIA_READ.p0pf)
    {
        switch(GTIA_READ.p0pf)
        {
            case 0x01:
                OS.pcolr0 = GTIA_COLOR_WHITE;
            break;
            case 0x02:
                OS.pcolr0 = GTIA_COLOR_GREEN;
            break;
            case 0x04:
                OS.pcolr0 = GTIA_COLOR_BROWN;
            break;
        }

        GTIA_WRITE.hitclr = 0x01;
    }
    else
        OS.pcolr0 = GTIA_COLOR_LIGHTRED;

    for(idx = 0; idx < num_players; ++idx)
    {
        player = &players.one + sizeof(Player) * (u_short)idx;

        // Update the animation by changing the current active player sequence image
        if(OS.rtclok[2] > 8)  // Roughly rotate all four images every second
        {
            OS.rtclok[0] = OS.rtclok[1] = OS.rtclok[2] = 0x00;

            if(player->image_idx == 3)
                player->anim_dir = -1;

            if(player->image_idx == 0)
                player->anim_dir = 1;

            player->image_idx  = player->image_idx + player->anim_dir;

            player->dirty = 1;
        }

        // Position dynamics with limitations, keeps the player relative to the ULC of the vieport
        // and makes sure that it doesn't scroll off the screen.  The limitation code will probably
        // move to mechanics.h/c.
        if(player->dirty)
        {
            byte x, y;
            short screen_x, screen_y;

            // Only draw it on the screen.
            screen_x = (short)player->x - vp_ul.x;
            screen_y = (short)player->y - vp_ul.y;

            if(screen_x > PF_MAX_X)
                x = PF_MAX_X;
            else if(screen_x < PF_MIN_X)
                x = PF_MIN_X;
            else
                x = (byte)screen_x;

            if(screen_y > PF_MAX_Y)
                y = PF_MAX_Y;
            else if(screen_y < PF_MIN_Y)
                y = PF_MIN_Y;
            else
                y = (byte)screen_y;

            //cprintf("%d: %d, %d:%d, %d -> %d, %d     \r", idx, player->x, player->y, screen_x, screen_y, x, y);

            // Move in Y by updating the image in memory
            memcpy(&(player_missiles.player0[y]), &player_1_0[0] + sizeof(player_1_0) * player->image_idx, sizeof(player_1_0));

            // Move in X by updating the PM horizontal position
            *(&GTIA_WRITE.hposp0 + idx) = x;

            player->dirty = 0;
        }
    }
}