#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

void controller_init(void);
void controller_set_limits(int32_t left, int32_t right);
void controller_set_target(int8_t joystick_x);
void controller_update(void);
void controller_stop(void);
int32_t controller_get_target(void);

#endif