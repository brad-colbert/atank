#include <atari.h>

//extern void __fastcall__ scroll_playfield_fast(unsigned char len, void *src, void *dst); //void* dl_address, u_short row, u_short col);

extern void spf();
extern void setup_vbi();
extern void wait_wsync();
extern void playfield_init();
extern void fine_scroll_left();
extern void fine_scroll_right();
extern void fine_scroll_up();
extern void fine_scroll_down();

