import socket
import json
import time
from random import random
import struct
import binascii

# Server settings
HOST = "" #127.0.0.1"
PORT = 5000
BUFFER_SIZE = 1024
start_time = None

# Packet format: [type, client_id, x, y, sequence, timestamp]
'''
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
'''
header_unpacker = struct.Struct('I B B H')
entity_unpacker = struct.Struct('I B B H h h h h')

# Player data storage (player_id -> position, timestamp, last_seq)
player_data = {}

# Create UDP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((HOST, PORT))

print(f"Enhanced UDP MMO Server running on {HOST}:{PORT}")

def interpolate_position(old_pos, new_pos, alpha=0.9):
    """Interpolates between two positions for smoother movement."""
    return {
        "x": old_pos["x"] * (1 - alpha) + new_pos["x"] * alpha,
        "y": old_pos["y"] * (1 - alpha) + new_pos["y"] * alpha
    }

def get_time():
    global start_time
    if start_time is None:
        start_time = time.time()
    return time.time() - start_time

"""Returns the current time in jiffy units."""
def time_jiffy(type: bool) -> int:
    # PAL
    if type:
        return (int)(time.time() * 49.86)
    # NTSC
    else:
        return (int)(time.time() * 59.9227)
    #return (int)(time.time() * 49.86)   # PAL
    #return (int)(time.time() * 59.9227) # NTSC

def jiffy_to_time(jiffy: int, type: bool) -> float:
    # PAL
    if type:
        return (float)(jiffy / 49.86)
    # NTSC
    else:
        return (float)(jiffy / 59.9227)

def time_to_jiffy(time: float, type: bool) -> int:
    # PAL
    if type:
        return (int)(time * 49.86)
    # NTSC
    else:
        return (int)(time * 59.9227)
    
def fixed_point_to_float(fixed_point: int) -> float:
    """Converts a 16-bit signed fixed point number to a floating point number."""
    # Handle two's complement for negative values
    sign = 1
    if fixed_point & 0x8000:
        sign = -1
        fixed_point = ~(fixed_point-1)#-((~fixed_point & 0xFFFF) + 1)
    # Extract the 12-bit whole number part
    whole_number = (fixed_point >> 4) & 0xFFF
    # Extract the 4-bit fraction part
    fraction = fixed_point & 0xF
    # Combine the whole number and fraction
    return sign * (whole_number + (fraction / 16.0))

def float_to_fixed_point(value: float) -> int:
    """Converts a floating point number to a 16-bit signed fixed point number."""
    sign = 1
    if value < 0:
        sign = -1
        value = abs(value)
    whole_number = int(value)
    fraction = int((value - whole_number) * 16)
    fixed_point = (whole_number << 4) | (fraction & 0xF)
    if sign == -1:
        fixed_point = -fixed_point
    return fixed_point & 0xFFFF

real_start_time = loop_start = time.time()

# **NOTE** We convert all time (jiffy) and time based data (velocities pix/jiffy) to time (seconds) for simplicity
print("Starting at", get_time())
while True:
    try:
        '''
        '''
        # Keep track of time
        now = get_time()

        # Receive data from a client
        data, addr = server_socket.recvfrom(BUFFER_SIZE)

        data_size = len(data)
        player_id = str(addr)  # Unique ID based on client address

        # Just header (probably time sync)
        if 8 == data_size:
            unpacked_data = header_unpacker.unpack(data)
        # Entity packet
        elif 16 == data_size:
            unpacked_data = entity_unpacker.unpack(data)

        packet_header = { "rtclock": unpacked_data[0],
                          "msg_type": unpacked_data[1],
                          "ntsc_flag": unpacked_data[2],
                          "sequence": unpacked_data[3]
                        }
        
        timestamp = jiffy_to_time(packet_header["rtclock"], packet_header["ntsc_flag"])

        if packet_header["msg_type"] == ord('t'):      # Time sync
            print(f"Time sync: {get_time():.3f} ({jiffy_to_time(packet_header['rtclock'], packet_header['ntsc_flag']):.3f}) ", end='')
            # Send back the time T2
            t2_jiffy = time_to_jiffy(get_time(), packet_header["ntsc_flag"])
            server_socket.sendto(struct.pack('I B B H', t2_jiffy, ord('t'), 0, 1), addr)
            print(f"{t2_jiffy:08X} ", end='')

            # Send T3
            t3_jiffy = time_to_jiffy(get_time(), packet_header["ntsc_flag"])
            server_socket.sendto(struct.pack('I B B H', t3_jiffy, ord('t'), 0, 2), addr)
            print(f"{t3_jiffy:08X} ({packet_header['rtclock']:08X})")

        elif packet_header["msg_type"] == ord('m'):    # Movement
            sim_packet = { "header": packet_header,
                           "entity": {"x": fixed_point_to_float(unpacked_data[4]),
                                      "y": fixed_point_to_float(unpacked_data[5]),
                                      "vx": fixed_point_to_float(unpacked_data[6]),
                                      "vy": fixed_point_to_float(unpacked_data[7])} }            
            

            print(f'Data {player_id}: {sim_packet["entity"]["x"]:.1f},{sim_packet["entity"]["y"]:.1f} - {sim_packet["entity"]["vx"]:.1f},{sim_packet["entity"]["vy"]:.1f} {(now - timestamp):.3f}')

            sequence = sim_packet["header"]["sequence"]

            # Initialize player if first time
            if player_id not in player_data:
                player_data[player_id] = {"x": sim_packet["entity"]["x"], "y": sim_packet["entity"]["y"], "last_seq": -1, "last_time": timestamp}

            # Ignore old/out-of-order packets
            if sequence <= player_data[player_id]["last_seq"]:
                continue

            # Estimate player's real position using lag compensation
            ping_time = get_time() - timestamp
            estimated_x = sim_packet["entity"]["x"] + ping_time * jiffy_to_time(sim_packet["entity"]["vx"], sim_packet["header"]["ntsc_flag"])  # Convert pix/jiffy to pix/sec
            estimated_y = sim_packet["entity"]["y"] + ping_time * jiffy_to_time(sim_packet["entity"]["vy"], sim_packet["header"]["ntsc_flag"])
            print(f"Estimate {player_id}: {estimated_x:.1f},{estimated_y:.1f}")

            # Interpolate between previous and estimated position
            smooth_pos = interpolate_position(player_data[player_id], {"x": estimated_x, "y": estimated_y})

            # Update player position
            player_data[player_id]["x"] = smooth_pos["x"]
            player_data[player_id]["y"] = smooth_pos["y"]
            player_data[player_id]["last_seq"] = sequence
            player_data[player_id]["last_time"] = timestamp
            print(f"Smoothed {player_id}: {smooth_pos['x']:.1f},{smooth_pos['y']:.1f}")
            print(f"T: {time_to_jiffy(timestamp, sim_packet['header']['ntsc_flag']):04X} S: {sequence:04X} X: {float_to_fixed_point(smooth_pos['x']):04x}, Y: {float_to_fixed_point(smooth_pos['y']):04x}")

            # Broadcast new positions to all players
            '''
            '''
            try:
                for client_addr in player_data.keys():
                    update_date = struct.pack('I h h h', time_to_jiffy(timestamp, sim_packet["header"]["ntsc_flag"]), sequence, float_to_fixed_point(smooth_pos['x']), float_to_fixed_point(smooth_pos['y']))
                    server_socket.sendto(update_date, eval(client_addr))
                    print('{!r}'.format(binascii.hexlify(update_date)))
            except Exception as e:
                print(f"Error: {e}")

        '''
        # Receive data from a client
        data, addr = server_socket.recvfrom(BUFFER_SIZE)

        unpacked_data = unpacker.unpack(data)
        sim_packet = { "rtclock": unpacked_data[0],
                       "sequence": unpacked_data[1],
                       "msg_type": unpacked_data[2],
                       "ntsc_flag": unpacked_data[3],
                       "entity": {"x": fixed_point_to_float(unpacked_data[4]),
                                  "y": fixed_point_to_float(unpacked_data[5]),
                                  "vx": fixed_point_to_float(unpacked_data[6]),
                                  "vy": fixed_point_to_float(unpacked_data[7])} }            
        client_time = sim_packet["rtclock"]

        # Have to correct time to be in line with the rtclock on the atari
        now = time.time()
        if start_time is None:
            start_time = time_jiffy(sim_packet["ntsc_flag"]) - client_time
        timestamp = time_jiffy(sim_packet["ntsc_flag"]) - start_time
        drift = timestamp - client_time  # in jiffys
        drift_t = drift/((now - real_start_time)/60.0)   # jiffy's per minute

        delta_time = now - loop_start
        loop_start = now
        print('{!r}'.format(binascii.hexlify(data)) + f' {sim_packet["rtclock"]}, {sim_packet["sequence"]}, drift:{drift} drift_t:{drift_t:.3f} d:{delta_time:.3f} t:{now - real_start_time:.3f}', )
        print(f'position: {sim_packet["entity"]["x"]},{sim_packet["entity"]["y"]}',
              f'velocity: {sim_packet["entity"]["vx"]},{sim_packet["entity"]["vy"]}')
        '''


        # send back the time
        #update_time = timestamp
        #packed_data = unpacker.pack(update_time, unpacked_data[1])
        #server_socket.sendto(packed_data, addr)

    except Exception as e:
        print(f"Error: {e}")
