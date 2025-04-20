#include "map.h"
#include "shapes.h"

#include <conio.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define EOFx 0xFF

// 32 lines => 64 coordinates => 256 bytes
#define MAX_LINES 32
Line lines[MAX_LINES];

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

void loadMap(const char* filename, uint8_t* lineCount) {
    FILE* file;
    //int capacity = 10;
    char buff[128];
    //cprintf("Loading map from file: %s\n\r", filename);

    file = fopen(filename, "r");

    if (!file) {
        cprintf("Failed to open file");
        exit(-1);
    }

    (*lineCount) = 0;

     while (readLine(file, buff, sizeof(buff)) && *lineCount < MAX_LINES) {
        char shape;
        int16_t x1, y1, x2, y2;
        
        //printf("line: %d %s", strlen(line), line);
        //printf("\n");
        if (sscanf(buff, " %c %d,%d %d,%d", &shape, &x1, &y1, &x2, &y2) == 5) {
            //printf("shape: %d %c, x1: %d, y1: %d, x2: %d, y2: %d\n", *lineCount, shape, x1, y1, x2, y2);

            switch(shape)
            {
                case 'L':
                    setLine(&lines[(*lineCount)++], x1, y1, x2, y2);
                break;
                case 'S':
                    // Convert square to four lines
                    setLine(&lines[(*lineCount)++], x1, y1, x2, y1);
                    setLine(&lines[(*lineCount)++], x2, y1, x2, y2);
                    setLine(&lines[(*lineCount)++], x1, y1, x1, y2);
                    setLine(&lines[(*lineCount)++], x1, y2, x2, y2);
                break;
                default:
                break;
            }
        }
    }

    fclose(file);
}

void drawMap(uint8_t lineCount) {
    uint8_t i;
    for (i = 0; i < lineCount; ++i) {
        drawLine(&lines[i]);
    }
}