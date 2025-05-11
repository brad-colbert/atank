#include "shapes.h"

// This converts two's complement signed integers to simply sign bit signed integers for the clipping code.
uint16_t fix_neg(uint16_t val)
{
    if (val < 0){
        uint8_t* access = (uint8_t*)&val;
        val = -val;
        return (access[1] | 0x80);
    }

    return val;
}

void setPoint(Point* P, int16_t X, int16_t Y)
{
    P->x = fix_neg(X);
    P->y = fix_neg(Y);
}

void setLine(Line* L, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2)
{
    setPoint (&L->start, X1, Y1);
    setPoint (&L->end, X2, Y2);
}

void setSquare(Square* S, int16_t X, int16_t Y, int16_t W, int16_t H)
{
    setPoint (&S->ll, X, Y);
    setPoint (&S->ul, X, Y + H);
    setPoint (&S->ur, X + W, Y + H);
    setPoint (&S->lr, X + W, Y);
}