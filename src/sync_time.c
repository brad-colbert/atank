#include <fujinet-network.h>

#include <atari.h>
#include <conio.h>

// Takes an array of 4 bytes and converts it to a 32-bit integer
#define as_int32(x) (*((int32_t*)x))

typedef struct _time_packet {
    uint8_t rtclock[4];
    uint8_t msg_type;
    uint8_t ntsc_flag; // 0 for NTSC, 1 for PAL
    uint16_t sequence;
} TimePacket;

uint8_t t1[4], t2[4], t3[4], t4[4];

void sync_time(const char* url)
{
    int32_t rtt, offset, t2_t1, t3_t4, t21_t34;
    int16_t num_bytes;
    TimePacket time_packet;
    time_packet.msg_type = 't';
    time_packet.ntsc_flag = get_tv();
    time_packet.sequence = 0;

    // T1
    time_packet.rtclock[0] = t1[0] = OS.rtclok[2];
    time_packet.rtclock[1] = t1[1] = OS.rtclok[1];
    time_packet.rtclock[2] = t1[2] = OS.rtclok[0];
    time_packet.rtclock[3] = t1[3] = 0;

    // Send the a packet to the server.  Doesn't matter what is in it.  It just triggers
    // the server to send one back with it's time.
    if(FN_ERR_OK != network_write(url, (uint8_t*)&time_packet, sizeof(TimePacket)))
    {
        cprintf("Unable to write time request\n\r");
        return;
    }

    // Read T2 from server (represents round trip time)
    num_bytes = network_read(url, (uint8_t*)&time_packet, sizeof(TimePacket));
    if(num_bytes < sizeof(TimePacket))
    {
        cprintf("Unable to read T2 request\n\r");
        return;
    }

    if(time_packet.sequence != 1)
    {
        cprintf("Invalid sequence number !1 %d\n\r", time_packet.sequence);
        return;
    }

    t2[0] = time_packet.rtclock[0];
    t2[1] = time_packet.rtclock[1];
    t2[2] = time_packet.rtclock[2];
    t2[3] = 0;

    // Read T3 from server (represents server to client time)
    num_bytes = network_read(url, (uint8_t*)&time_packet, sizeof(TimePacket));
    if(num_bytes < sizeof(TimePacket))
    {
        cprintf("Unable to read T3 request\n\r");
        return;
    }

    // T4  (get as soon as possible)
    t4[0] = OS.rtclok[2];
    t4[1] = OS.rtclok[1];
    t4[2] = OS.rtclok[0];
    t4[3] = 0;

    if(time_packet.sequence != 2)
    {
        cprintf("Invalid sequence number !2 %d\n\r", time_packet.sequence);
        return;
    }

    // T3
    t3[0] = time_packet.rtclock[0];
    t3[1] = time_packet.rtclock[1];
    t3[2] = time_packet.rtclock[2];
    t3[3] = 0;

    // Calculate the round trip time
    //rtt = (as_int32(t4) - as_int32(t1)) - (as_int32(t3) - as_int32(t2));
    t2_t1 = as_int32(t2) - as_int32(t1);
    t3_t4 = as_int32(t3) - as_int32(t4);
    t21_t34 = t2_t1 - t3_t4;
    offset = t21_t34 / (int32_t)2;

    // Correct the time.  Set it to the last server time, plus the offset.
    as_int32(t3) = as_int32(t3) + offset;
    OS.rtclok[0] = t3[2];
    OS.rtclok[1] = t3[1];
    OS.rtclok[2] = t3[0];

    // cprintf("A:%08X B:%08X\n\r", as_int32(t1), as_int32(t2));
    // cprintf("C:%08X D:%08X\n\r", as_int32(t3), as_int32(t4));

    //cprintf("RTT: %ld, Offset: %ld\n\r", rtt, offset);
    //cprintf("T: %ld Offset: %ld\n\r", as_int32(t4), offset);
}