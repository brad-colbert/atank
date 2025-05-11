#include "map.h"
#include "shapes.h"
#include "graphics.h"
#include "draw.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define EOFx 0xFF

// Externals

// Globals
uint8_t line_count = 0;

// 32 lines => 64 coordinates => 256 bytes
#define MAX_LINES 32
#pragma bss-name (push,"LINE_COORDS")
Line lines[MAX_LINES];
#pragma bss-name (pop)
#pragma data-name (push,"ZEROPAGE")
Point map_center = {0, 0};
Point map_center_prev = {0, 0};
#pragma data-name (pop)


char* readLine(FILE* file, char* buffer, int maxBytes) {
    int i = 0;
    char ch;
    while (i < maxBytes - 1 && (ch = fgetc(file)) != EOFx && ch != 10) {
        buffer[i++] = ch;
    }
    buffer[i] = NULL;
    if (ch == EOFx) {
        return NULL;
    }

    return buffer;
}

void load_map(const char* filename, uint8_t* line_count) {
    FILE* file;
    char buff[128];

    file = fopen(filename, "r");

    if (!file) {
        printf("Failed to open file");
        exit(-1);
    }

    (*line_count) = 0;

     while (readLine(file, buff, sizeof(buff)) && *line_count < MAX_LINES) {
        char shape;
        if (sscanf(buff, "%c", &shape) != 1) {
            continue;
        }
        else {
            int16_t x1, y1, x2, y2;
            
            if ('#' == shape) {
                // Skip comment line
                continue;
            }

            if (sscanf(buff, "%c %d,%d %d,%d", &shape, &x1, &y1, &x2, &y2) == 5) {
                // Offset coordinates to center of screen
                x1 += WIDTH_PIXELS/2;
                x2 += WIDTH_PIXELS/2;
                y1 += HEIGHT_PIXELS/2;
                y2 += HEIGHT_PIXELS/2;

                switch(shape)
                {
                    case 'L':
                        setLine(&lines[(*line_count)++], x1, y1, x2, y2);
                    break;
                    case 'S':
                        // Convert square to four lines
                        setLine(&lines[(*line_count)++], x1, y1, x2, y1);
                        setLine(&lines[(*line_count)++], x2, y1+1, x2, y2);   // So they don't overlap
                        setLine(&lines[(*line_count)++], x1, y1+1, x1, y2);   // So they don't overlap
                        setLine(&lines[(*line_count)++], x1+1, y2, x2-1, y2); // So they don't overlap
                    break;
                    default:
                    break;
                }
            }
        }
    }

    fclose(file);
}

void printLines(uint8_t line_count) {
    uint8_t i;
    for (i = 0; i < line_count; ++i) {
        printf("Line %d: (%d, %d) to (%d, %d)\n", i, lines[i].start.x, lines[i].start.y, lines[i].end.x, lines[i].end.y);
    }
}

void set_map_center(Point* center) {
    // Save the previous center
    map_center_prev = map_center;

    // We use map_center to offset the coordinates to properly position the map on the screen.
    // This is why we use the negative of the center coordinates.
    map_center.x = -center->x;
    map_center.y = -center->y;
}

void draw_map() {
    // Do not draw the map if the center has not changed
    if (map_center.x == map_center_prev.x && map_center.y == map_center_prev.y) {
        return;
    }
    translate_clip_draw_all_lines();
}
