#ifndef __LINE_CLIPPING_H__
#define __LINE_CLIPPING_H__

#include <stdint.h>

void cdecl clip(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void fastcall test_carry_and_store(void);

#endif