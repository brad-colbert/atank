#include "shapes.h"

//#include <tgi.h>

void setPoint(Point* P, int16_t X, int16_t Y)
{
    P->x = X;
    P->y = Y;
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

void translateLine(Line* L, int X, int Y)
{
    L->start.x += X;
    L->start.y += Y;
    L->end.x += X;
    L->end.y += Y;
}

void drawLine(const Line* L)
{
    //tgi_line(L->start.x, L->start.y, L->end.x, L->end.y);
}

void translateSquare(Square* S, int X, int Y)
{
    S->ll.x += X;
    S->ll.y += Y;
    S->ul.x += X;
    S->ul.y += Y;
    S->ur.x += X;
    S->ur.y += Y;
    S->lr.x += X;
    S->lr.y += Y;
}

void drawSquare(const Square* S)
{
/*
    tgi_line (S->ll.x, S->ll.y, S->ul.x, S->ul.y);
    tgi_line (S->ul.x, S->ul.y, S->ur.x, S->ur.y);
    tgi_line (S->ur.x, S->ur.y, S->lr.x, S->lr.y);
    tgi_line (S->lr.x, S->lr.y, S->ll.x, S->ll.y);
*/}
