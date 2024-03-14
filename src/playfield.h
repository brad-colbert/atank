#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include "types.h"

#define PF_PAGE_ROWS 4
#define PF_PAGE_COLS 4

#ifdef GFX7
#else
#define PF_ROW_TILES 24
#define PF_COL_TILES 40

#define PF_COURSE_ROWS (PF_ROW_TILES * PF_PAGE_ROWS)
#define PF_COURSE_COLS (PF_COL_TILES * PF_PAGE_COLS)

#define PF_ROW_PIX 8
#define PF_COL_PIX 4

#define PF_LINES (PF_ROW_PIX * PF_COURSE_ROWS)
#define PF_COLS  (PF_COL_PIX * PF_COURSE_COLS)

#define PF_LINES_PER_PAGE (PF_ROW_TILES * PF_ROW_PIX)
#define PF_COLS_PER_PAGE (PF_COL_TILES * PF_COL_PIX)
#endif

// There could be an issue where adding the number of skip lines to the
// playfield buffer makes you cross another 4K boundary... we'll deal with
// that later.
//#define NUM_ROWS_TO_SKIP (PF_COURSE_ROWS / (4096 / PF_COURSE_COLS))
#define NUM_4K_OVERLAPS ((PF_COURSE_ROWS * PF_COURSE_COLS / 4096) - 1)

extern byte playfield[100][160]; //[PF_COURSE_ROWS + NUM_4K_OVERLAPS][PF_COURSE_COLS];
//extern byte playfield[][];
extern coord vp_ul, vp_lr;

void init_playfield(void);

void set_playfield_viewport(u_short x, u_short y);

#endif // __PLAYFIELD_H__