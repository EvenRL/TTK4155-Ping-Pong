#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <avr/pgmspace.h>
#include <stdint.h>

#define WASTED_WIDTH 105
#define WASTED_HEIGHT 24
#define WASTED_PAGES 3

#define PING_PONG_FRAME_COUNT 5
#define PING_PONG_FRAME_BYTES 1024

extern const uint8_t font8[95][8] PROGMEM;
extern const uint8_t font5[95][5] PROGMEM;
extern const uint8_t font4[95][4] PROGMEM;
extern const uint8_t wasted_bitmap[WASTED_PAGES][WASTED_WIDTH] PROGMEM;
extern const uint8_t image[1024] PROGMEM;
extern const uint8_t
ping_pong_frames[PING_PONG_FRAME_COUNT][PING_PONG_FRAME_BYTES]
PROGMEM;

#endif