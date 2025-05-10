#ifndef MULTIPLY16_H
#define MULTIPLY16_H

#include <stdint.h>

#define FIXED_POINT 4 // Define Q12.4 fixed-point type
#define ONE_FIXED (1 << FIXED_POINT)
#define MASK_FIXED (ONE_FIXED - 1)
#define GET_BYTE(var, idx) (((uint8_t*)&var)[idx])

// Full result is stored in   int32_t product
void cdecl mul16(int16_t a, int16_t b);
int16_t int_to_fixed(int16_t value);
int16_t fixed_to_int(int16_t value);
int16_t get_product();

// Print fractional part in decimal (without multiplication/division)
void print_fractional_decimal(uint8_t fraction);

// Print fractional part in binary
void print_fractional_binary(uint8_t fraction);

// Print a fixed-point number in full
//void print_fixed(const char *label, fixed_t value) {
void print_fixed(int16_t value);

// Function to print a 16-bit number in binary form
void print_binary(uint16_t n);
#endif