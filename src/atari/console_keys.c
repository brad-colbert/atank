// (C) 2025 Brad Colbert

#include "console_keys.h"

uint8_t read_console_keys(const uint8_t console_key) {
    // Read the console keys from the memory location
    return !(*(volatile uint8_t*)CONSOLE_MEM_LOCATION & console_key);
}