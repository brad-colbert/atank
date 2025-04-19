#ifndef __DRAWLINE_H__
#define __DRAWLINE_H__

#include <stdint.h>

//void cdecl setPixel(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//void setPixelC(uint16_t oft, uint8_t b);
//void setPixelC(uint16_t oft);
void setPixelC(uint16_t x, uint8_t y);
//void cdecl setPixel(uint16_t offset, uint8_t bit);
void cdecl setPixel(uint16_t addr);
void cdecl setPixelXY(uint16_t x, uint8_t y);
void cdecl setPixelXYmask(uint8_t x, uint8_t y, uint8_t mask);

uint8_t fastcall calcLeftMask(uint8_t mask);
uint8_t fastcall calcRightMask(uint8_t mask);
 
//void cdecl XORLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void XORLineC(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void drawLineB(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#endif // __DRAWLINE_H__