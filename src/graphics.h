#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

// 7696 bytes
#define WIDTH_PIXELS 320
#define HEIGHT_PIXELS 192
#define WIDTH_BYTES (WIDTH_PIXELS / 8)
#define FRAMEBUFFER_SIZE (WIDTH_BYTES * HEIGHT_PIXELS)

void __fastcall__ init_graphics();

void __fastcall__ clear_graphics();

void __fastcall__ shutdown_graphics();

#endif