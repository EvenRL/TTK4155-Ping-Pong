#ifndef IO_H_
#define IO_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t right;
    uint8_t left;
    uint8_t nav;
} io_buttons_t;

io_buttons_t IO_ReadButtons(void);
bool IO_R6_Pressed(void);

void IO_SetLED(uint8_t led, bool on);

#endif