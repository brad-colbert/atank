#ifndef __TIMING_H__
#define __TIMING_H__

#include <atari.h>

#define TIME (OS.rtclok[1] * 256 + OS.rtclok[2])
#define TIME_RESET (OS.rtclok[1] = OS.rtclok[2] = 0)

#endif 