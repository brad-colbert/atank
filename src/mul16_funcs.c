#include <mul16.h>

#include <conio.h>

extern int32_t product; // 32-bit product of two 16-bit numbers computed in mul16

// Convert integer to Q8.8 fixed-point (uses two’s complement)
int16_t int_to_fixed(int16_t value) {
    return value << FIXED_POINT;  // Shift left (multiply by 256)
}

// Convert fixed-point to integer (extract whole part)
int16_t fixed_to_int(int16_t value) {
    return value >> FIXED_POINT;  // Shift right (divide by 256, preserves sign)
}

int16_t get_product()
{
    uint32_t temp = (uint32_t)product << FIXED_POINT;
    return (int16_t)(((uint16_t)GET_BYTE(temp, 2) << 8) + (uint16_t)GET_BYTE(temp, 1));
}

// Print fractional part in decimal (without multiplication/division)
void print_fractional_decimal(uint8_t fraction) {
    cprintf(".");

    // Extract each binary fraction manually
    #ifdef FIXED_POINT == 4
    if (fraction & 0x08) cprintf("5"); else cprintf("0");  // 0.03125
    if (fraction & 0x04) cprintf("2"); else cprintf("0");  // 0.015625
    if (fraction & 0x02) cprintf("1"); else cprintf("0");  // 0.0078125
    if (fraction & 0x01) cprintf("5"); else cprintf("0");  // 0.00390625
    #else
    if (fraction & 0x80) printf("5"); else printf("0");  // 0.5
    if (fraction & 0x40) printf("2"); else printf("0");  // 0.25
    if (fraction & 0x20) printf("1"); else printf("0");  // 0.125
    if (fraction & 0x10) printf("5"); else printf("0");  // 0.0625
    if (fraction & 0x08) printf("3"); else printf("0");  // 0.03125
    if (fraction & 0x04) printf("1"); else printf("0");  // 0.015625
    if (fraction & 0x02) printf("6"); else printf("0");  // 0.0078125
    if (fraction & 0x01) printf("9"); else printf("0");  // 0.00390625
    #endif
}

// Print fractional part in binary
void print_fractional_binary(uint8_t fraction) {
    int i;
    cprintf(" (binary: 0.");
    for (i = MASK_FIXED; i >= 0; i--) {
        if (fraction & (1 << i)) {
            cprintf("1");
        } else {
            cprintf("0");
        }
    }
    cprintf(")");
}

// Print a fixed-point number in full
//void print_fixed(const char *label, fixed_t value) {
void print_fixed(int16_t value) {
    int16_t whole = fixed_to_int(value);
    uint8_t fractional = value & (uint8_t)MASK_FIXED;//0xFF; //(uint8_t)(value & 0xFF00); // Extract lower 8 bits

    cprintf("%d", whole);
    print_fractional_decimal(fractional);
}

// Function to print a 16-bit number in binary form
void print_binary(uint16_t n) {
    int i ;
    for (i = (int)MASK_FIXED; i >= 0; i--) {
        cprintf("%d", (n >> i) & 1);
    }
    //printf("\n");
}