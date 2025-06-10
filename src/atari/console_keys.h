// (C) 2025 Brad Colbert

#ifndef __ATARI_CONSOLE_KEYS_H__
#define __ATARI_CONSOLE_KEYS_H__

#include <stdint.h>

#define CONSOLE_KEY_OPTION 0x04
#define CONSOLE_KEY_SELECT 0x02
#define CONSOLE_KEY_START  0x01

#define CONSOLE_MEM_LOCATION 0xD01F

uint8_t read_console_keys(const uint8_t console_key);