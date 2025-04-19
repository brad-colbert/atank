#include "drawline.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define WIDTH 320
#define HEIGHT 192
#define WIDTH_BPP 8
#define WIDTH_BYTES (WIDTH / WIDTH_BPP)

uint8_t bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

extern uint8_t framebuffer[7696]; // Assume framebuffer is defined elsewhere

void cohen_sutherland(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Cohen-Sutherland algorithm implementation
}

// Function to set a pixel at (x, y)
void setPixelC(uint16_t x, uint8_t y)
{
    setPixelXY(x, y);
}

// Function to XOR a line from (x1, y1) to (x2, y2) using Bresenham's line algorithm
// Note: This function assumes a function setPixel(x, y) that sets the pixel at (x, y)
void XORLineC(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t idx_y;
    uint16_t offset;
    uint8_t b, idx, x1b, x2b, y1b, y2b;

    // Check for horizontal line
    if (y1 == y2) {
        x1b = (uint8_t)(x1 / 8);
        x2b = (uint8_t)(x2 / 8);
        y1b = (uint8_t)y1;
        y2b = (uint8_t)y2;
        // We get to use a serious optimization here.  We get to write full bytes at a time.
        // Thus, cutting down the amount of writes by a factor of 8.

        // Write the left most pixel 2^bit+(2^bit-1)
        b = (uint8_t)(x1 & 0x07); // Get the bit position of the first pixel
        b = calcLeftMask(b);
        setPixelXYmask(x1b, (uint8_t)y1, b);

        for (idx = (uint8_t)(x1b+1); idx < (uint8_t)(x2b); ++idx) {
            // XOR the pixel at (x, y1)
            // Assuming a function setPixel(x, y) that sets the pixel at (x, y)
            // offset = y1 * WIDTH + idx_y;
            // setPixel(offset/8, (uint8_t)(offset%8)); // Replace with actual pixel manipulation code
            setPixelXYmask(idx, (uint8_t)y1, 0xFF);
        }

        // Write the right most pixel
        b = (uint8_t)(x2 & 0x07); // Get the bit position of the first pixel
        b = calcRightMask(b);
        setPixelXYmask(x2b, (uint8_t)y1, b);
    }
    // Check for vertical line
    else if (x1 == x2) {
        for (idx_y = y1; idx_y <= y2; ++idx_y) {
            setPixelC(x1, (uint8_t)idx_y); // Replace with actual pixel manipulation code
        }
    }/*
    // Diagonal line (Bresenham's line algorithm)
    else {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        int err2 = 0;

        while (1) {
            // XOR the pixel at (x1, y1)
            // Assuming a function setPixel(x, y) that sets the pixel at (x, y)
            setPixelC(x1, y1); // Replace with actual pixel manipulation code

            if (x1 == x2 && y1 == y2) break;
            err2 = err * 2;
            if (err2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (err2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }*/
    #if 0
    // Alternative simple implementation of Bresenham's line algorithm
    // Uncomment the following code for a simple implementation of Bresenham's line algorithm
    // Note: This is commented out to avoid redundancy with the above implementation
    // #if 0
    // This is a simple implementation of Bresenham's line algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        // XOR the pixel at (x1, y1)
        // Assuming a function setPixel(x, y) that sets the pixel at (x, y)
        setPixel(x1, y1); // Replace with actual pixel manipulation code

        if (x1 == x2 && y1 == y2) break;
        int err2 = err * 2;
        if (err2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    #endif
}

void drawLineB(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t b, idx;

    // Check for horizontal line
    if (y1 == y2) {
        // We get to use a serious optimization here.  We get to write full bytes at a time.
        // Thus, cutting down the amount of writes by a factor of 8.

        // Write the left most pixel 2^bit+(2^bit-1)
        b = (uint8_t)(x1 & 0x07); // Get the bit position of the first pixel
        b = calcLeftMask(b);
        setPixelXYmask(x1, y1, b);

        for (idx = x1+1; idx < x2; ++idx) {
            setPixelXYmask(idx, y1, 0xFF);
        }

        // Write the right most pixel
        b = (uint8_t)(x2 & 0x07); // Get the bit position of the first pixel
        b = calcRightMask(b);
        setPixelXYmask(x2, y1, b);
    }
    // Check for vertical line
    else if (x1 == x2) {
        for (idx = y1; idx <= y2; ++idx) {
            setPixelC(x1, idx); // Replace with actual pixel manipulation code
        }
    }
}
