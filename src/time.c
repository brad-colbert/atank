// (C) 2025 Brad Colbert

#include "time.h"

#include <stdint.h>

#pragma data-name (push,"ZEROPAGE")
uint32_t time_millis = 0x00000000;
#pragma data-name (pop)
