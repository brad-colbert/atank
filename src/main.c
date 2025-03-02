#include <mul16.h>

#include <fujinet-network.h>

#include <atari.h>
#include <conio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
typedef struct _update_packet {
    uint8_t rtclock[4];
    uint16_t sequence;
    int16_t x, y;
} UpdatePacket;

extern int32_t product; // 32-bit product of two 16-bit numbers computed in mul16

SimPacket sim_packet;
UpdatePacket update_packet;

int main(int argc, char* argv[])
{
    uint16_t num_bytes = 0;
    int16_t start_t, end_t, dt;
    int16_t update_now = 0, update_time = 0, update_dt = 0;
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

        // Send the entity state to the server
        #define READ_CORRECTION
        #ifdef READ_CORRECTION
        num_bytes = network_read_nb(URL, (uint8_t*)&update_packet, sizeof(UpdatePacket));
        if(num_bytes < sizeof(UpdatePacket))
        {
            printf("Unable to read request\n\r");
        }
        else
        {
            /*
            server_x = server_players[str((SERVER_IP, SERVER_PORT))]["x"]
            server_y = server_players[str((SERVER_IP, SERVER_PORT))]["y"]

            # Calculate time since last update
            now = time.time()
            dt = now - last_update_time

            # Extrapolate position based on velocity and elapsed time
            extrapolated_x = server_x + vx * dt
            extrapolated_y = server_y + vy * dt

            # Correct position if the server state differs significantly
            if abs(server_x - predicted_pos["x"]) > 2 or abs(server_y - predicted_pos["y"]) > 2:
                print(f"Correction applied: Server ({server_x}, {server_y}) vs Predicted ({predicted_pos['x']}, {predicted_pos['y']})")
                x, y = server_x, server_y  # Adjust to server state
            else:
                print("Prediction was accurate. Applying extrapolation.")
                x, y = extrapolated_x, extrapolated_y

            last_update_time = now
            print(f"Updated position (after extrapolation): ({x:.2f}, {y:.2f})")
            */
            int16_t ex_x, ex_y;

            // Calculate time since last update
            update_now = (int16_t)TIME;
            update_dt = update_now - update_time;
            update_dt = update_dt << FIXED_POINT;

            // Extrapolate position based on velocity and elapsed time
            mul16(sim_packet.entity.vx, update_dt);
            ex_x = update_packet.x + get_product();
            mul16(sim_packet.entity.vy, update_dt);
            ex_y = update_packet.y + get_product();

            // Correct position if the server state differs significantly
            if(abs(update_packet.x - ex_x) > (7<<FIXED_POINT) || abs(update_packet.y - ex_y) > (7<<FIXED_POINT))
            {
                //cprintf("Correction applied: Server (%04X, %04X) vs Predicted (%04X, %04X)\n\r", update_packet.x, update_packet.y, ex_x, ex_y);
                // cprintf("%04x:%04X-%04X=%04X ", update_dt, update_packet.x, ex_x, update_packet.x - ex_x);
                //cprintf("%04X-%04X=%04X:%04x ", update_now, update_time, update_dt, abs(update_packet.x - ex_x));
                //cputs("*");
                sim_packet.entity.x = update_packet.x;
                sim_packet.entity.y = update_packet.y;
            }
            else
            {
                //cprintf("Prediction was accurate. Applying extrapolation.\n\r");
                //cputs(". ");
                sim_packet.entity.x = ex_x;
                sim_packet.entity.y = ex_y;
            }

            update_time = update_now;
            //cprintf("Updated position (after extrapolation): (%04X, %04X)\n\r", sim_packet.entity.x, sim_packet.entity.y);
        }
        #endif

        end_t = (int16_t)TIME;
    }

    return 0;
}