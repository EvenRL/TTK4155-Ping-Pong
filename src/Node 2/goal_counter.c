#include "ir.h"
#include "goal_counter.h"

#include <stdint.h>

uint16_t threshold = 1500; // Example threshold value for IR sensor


// IF IR sensor value is below threshold, increment goal counter
void check_goal(uint16_t ir_value, uint16_t threshold, uint8_t *goal_counter) {
    static uint16_t previous_ir_value = 0;
    if (ir_value < threshold && previous_ir_value >= threshold) {
        (*goal_counter)++;
    }
    previous_ir_value = ir_value;
}


