#ifndef __MAP_H__
#define __MAP_H__

#include "shapes.h"

void load_map(const char* filename, uint8_t* line_count);

void set_map_center(Point* center);

#endif