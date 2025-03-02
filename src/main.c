#include <mul16.h>

#include <fujinet-network.h>

#include <atari.h>
#include <conio.h>

#include <stdio.h>
#include <string.h>

#define TIME (((uint16_t)OS.rtclok[1] << 8) + (uint16_t)OS.rtclok[2])
#define URL "N:UDP://192.168.1.205:5000"

typedef struct _entity {
    int16_t x, y;
    int16_t vx, vy;   // velocity in pixel/second (1 second is 60 frames NTSC, 50 frames PAL)
} Entity;
typedef struct _sim_packet {
    uint8_t rtclock[4];
    uint16_t sequence;
    uint8_t msg_type;
    uint8_t ntsc_flag; // 0 for NTSC, 1 for PAL
    Entity entity;
} SimPacket;

extern int32_t product; // 32-bit product of two 16-bit numbers computed in mul16

SimPacket sim_packet;

int main(int argc, char* argv[])
{
    int16_t start_t, end_t, dt;
    int8_t dir = 1;
    uint8_t b = 0;

    sim_packet.ntsc_flag = get_tv();
    sim_packet.entity.x = 0x0000;
    sim_packet.entity.y = 0x0000;
    sim_packet.entity.vx = (int16_t)3 << (FIXED_POINT-1);   // 1 pix per frame
    sim_packet.entity.vy = 0x0000;

    OS.rtclok[2] = 0;
    OS.rtclok[1] = 0;
    OS.rtclok[0] = 0;

    printf("Starting the test client\n");

    if(FN_ERR_OK != network_init())
    {
        printf("Failed to initialize network\n\r");
        return 0x0;
    }

    if(FN_ERR_OK != network_open(URL, 22, 0))
    {
        printf("Failed to open URL\n\r");
        return 0x0;
    }
        
    start_t = 0;
    end_t = 0;
    dt = 0;
    while(1)
    {
        dt = end_t - start_t;
        dt = dt << FIXED_POINT;  // convert to current fixed-point format
        //cprintf("%u:", start_t);
        start_t = (int16_t)TIME;

        sim_packet.msg_type = 'm';
        sim_packet.rtclock[0] = OS.rtclok[2];
        sim_packet.rtclock[1] = OS.rtclok[1];
        sim_packet.rtclock[2] = OS.rtclok[0];
        sim_packet.rtclock[3] = 0;
        ++sim_packet.sequence;

        // cprintf("x:");
        // print_fixed(sim_packet.entity.x);
        // cprintf("+");
        // print_fixed(sim_packet.entity.vx);
        // cprintf("*");
        // print_fixed(dt);
        // cprintf("=");
        mul16(sim_packet.entity.vx, dt);
        sim_packet.entity.x = sim_packet.entity.x + get_product();
        // print_fixed(sim_packet.entity.x);
        // cprintf("   \r\n");

        // cprintf("y:");
        // print_fixed(sim_packet.entity.y);
        // cprintf("+");
        // print_fixed(sim_packet.entity.vy);
        // cprintf("*");
        // print_fixed(dt);
        // cprintf("=");
        mul16(sim_packet.entity.vy, dt);
        sim_packet.entity.y = sim_packet.entity.y + get_product();
        // print_fixed(sim_packet.entity.y);
        // cprintf("   \r\n");

        // 0x7FF0
        #define VALUE_THRESH 0x7D00
        if(sim_packet.entity.x > VALUE_THRESH)
        {
            sim_packet.entity.x = VALUE_THRESH;
            sim_packet.entity.vy = sim_packet.entity.vx;
            sim_packet.entity.vx = 0;
        }
        else if(sim_packet.entity.y > VALUE_THRESH){
            sim_packet.entity.y = VALUE_THRESH;
            sim_packet.entity.vx = -sim_packet.entity.vy;
            sim_packet.entity.vy = 0;
        }
        else if(sim_packet.entity.x < 0x0000){
            sim_packet.entity.x = 0x0000;
            sim_packet.entity.vy = sim_packet.entity.vx;
            sim_packet.entity.vx = 0;
        }
        else if(sim_packet.entity.y < 0x0000){
            sim_packet.entity.y = 0x0000;
            sim_packet.entity.vx = -sim_packet.entity.vy;
            sim_packet.entity.vy = 0;
        }

        // Send the entity state to the server
        if(FN_ERR_OK != network_write(URL, (uint8_t*)&sim_packet, sizeof(SimPacket)))
        {
            printf("Unable to write request\n\r");
        }
        else
        {
            //printf(">%d\n", entity.count);
        }        

        end_t = (int16_t)TIME;
    }

    return 0;
}