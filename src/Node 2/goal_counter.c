#include "goal_counter.h"
#include "time.h"

#include <stdint.h>
#include <stdbool.h>

#define IR_HYSTERESIS       100
#define IR_BLOCK_MS          8
#define IR_CLEAR_MS         30
#define GOAL_COOLDOWN_MS   150

void check_goal(
    uint16_t ir_value,
    uint16_t threshold,
    uint32_t *goal_counter)
{
    static bool armed = false;
    static bool pending = false;
    static bool has_goal = false;

    static uint64_t pending_since = 0;
    static uint64_t last_goal = 0;

    const uint16_t low =
        threshold > IR_HYSTERESIS
        ? threshold - IR_HYSTERESIS
        : 0;

    const uint16_t high =
        threshold <= UINT16_MAX - IR_HYSTERESIS
        ? threshold + IR_HYSTERESIS
        : UINT16_MAX;

    uint64_t now = time_now();

    // ---------------- WAIT FOR CLEAR BEAM ----------------

    if (!armed)
    {
        if (ir_value >= high)
        {
            if (!pending)
            {
                pending = true;
                pending_since = now;
            }
            else if (now - pending_since >= msecs(IR_CLEAR_MS))
            {
                armed = true;
                pending = false;
            }
        }
        else
        {
            pending = false;
        }

        return;
    }

    // ---------------- DETECT BLOCKED BEAM ----------------

    if (ir_value <= low)
    {
        if (!pending)
        {
            pending = true;
            pending_since = now;
        }
        else if (now - pending_since >= msecs(IR_BLOCK_MS))
        {
            armed = false;
            pending = false;

            // Count only if cooldown has expired
            if (!has_goal ||
                now - last_goal >= msecs(GOAL_COOLDOWN_MS))
            {
                (*goal_counter)++;

                last_goal = now;
                has_goal = true;
            }
        }
    }
    else
    {
        pending = false;
    }
}