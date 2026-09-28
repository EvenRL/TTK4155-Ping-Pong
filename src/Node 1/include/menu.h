
#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include "joysticktouch.h"

void menu_init(void);
void menu_update(uint8_t adc_values[4]);

#endif
