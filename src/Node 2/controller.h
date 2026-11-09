#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

void controller_init(void);
void controller_set_target(int8_t joystick_x);
void controller_update(void);

#endif