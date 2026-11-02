#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_init(void);
void pwm_set_pulse_width(uint16_t pulse_us);

#endif