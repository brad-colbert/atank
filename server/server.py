import socket
import json
import time
from random import random

# Server settings
HOST = "127.0.0.1"
PORT = 6789
BUFFER_SIZE = 1024

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

while True:
    try:
        # Receive data from a client
        time.sleep(random()) # Simulate random network delay
        data, addr = server_socket.recvfrom(BUFFER_SIZE)
        message = json.loads(data.decode())

        player_id = str(addr)  # Unique ID based on client address

        if message["type"] == "move":
            sequence = message["sequence"]
            timestamp = message["timestamp"]
            received_pos = {"x": message["x"], "y": message["y"]}

            # Initialize player if first time
            if player_id not in player_data:
                player_data[player_id] = {"x": received_pos["x"], "y": received_pos["y"], "last_seq": -1, "last_time": timestamp}

            # Ignore old/out-of-order packets
            if sequence <= player_data[player_id]["last_seq"]:
                continue

            # Estimate player's real position using lag compensation
            ping_time = time.time() - timestamp
            estimated_x = received_pos["x"] + ping_time * 1  # Assuming speed = 1 unit/sec
            estimated_y = received_pos["y"] + ping_time * 1
            print(f"Estimate {player_id}: {estimated_x},{estimated_y}")

            # Interpolate between previous and estimated position
            smooth_pos = interpolate_position(player_data[player_id], {"x": estimated_x, "y": estimated_y})

            # Update player position
            player_data[player_id]["x"] = smooth_pos["x"]
            player_data[player_id]["y"] = smooth_pos["y"]
            player_data[player_id]["last_seq"] = sequence
            player_data[player_id]["last_time"] = timestamp
            print(f"Smoothed {player_id}: {smooth_pos['x']},{smooth_pos['y']}")

            # Broadcast new positions to all players
            update_message = json.dumps({"type": "update", "players": player_data})
            for client_addr in player_data.keys():
                server_socket.sendto(update_message.encode(), eval(client_addr))

    except Exception as e:
        print(f"Error: {e}")
