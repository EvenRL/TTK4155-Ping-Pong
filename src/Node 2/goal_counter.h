#ifndef GOAL_COUNTER_H
#define GOAL_COUNTER_H

#include <stdint.h>

void check_goal(
    uint16_t ir_value,
    uint16_t threshold,
    uint32_t *goal_counter
);

#endif