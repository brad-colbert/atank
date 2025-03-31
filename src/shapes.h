#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <stdint.h>

typedef struct _Point {
    int16_t x;
    int16_t y;
} Point;

typedef struct _Line {
    Point start;
    Point end;
} Line;

typedef struct _Square {
    Point ll;
    Point ul;
    Point ur;
    Point lr;
} Square;

void setPoint(Point* P, int16_t X, int16_t Y);
void setLine(Line* L, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2);
void setSquare(Square* S, int16_t X, int16_t Y, int16_t W, int16_t H);

void translateLine(Line* L, int16_t X, int16_t Y);
void drawLine(const Line* L);

void translateSquare (Square* S, int16_t X, int16_t Y);
void drawSquare (const Square* S);

#endif