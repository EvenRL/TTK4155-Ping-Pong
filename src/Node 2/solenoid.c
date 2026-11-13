#include "solenoid.h"
#include "sam.h"
#include "time.h"
#include <stdint.h>
#include <stdbool.h>

// Solenoid control pin (Arduino Due D2)
#define SOLENOID_PIN PIO_PB25

// Timing
#define SOLENOID_PULSE_MS 40
#define SOLENOID_COOLDOWN_MS 300

static bool solenoid_active = false;
static bool has_fired = false;

static uint64_t pulse_start = 0;
static uint64_t last_shot = 0;


// Initialize solenoid output
void solenoid_init(void)
{
    // Enable PIOB clock
    PMC->PMC_PCER0 |= (1u << ID_PIOB);

    // Set output HIGH before enabling output
    // PNP transistor: HIGH = OFF
    PIOB->PIO_SODR = SOLENOID_PIN;

    // Configure PB25 as GPIO output
    PIOB->PIO_PER = SOLENOID_PIN;
    PIOB->PIO_OER = SOLENOID_PIN;

    solenoid_active = false;
    has_fired = false;
}


// Start a single shooting pulse
void solenoid_shoot(void)
{
    uint64_t now = time_now();

    // Ignore requests while shooting
    if (solenoid_active)
    {
        return;
    }

    // Prevent shooting too frequently
    if (has_fired &&
        now - last_shot < msecs(SOLENOID_COOLDOWN_MS))
    {
        return;
    }

    // Activate solenoid (active LOW)
    PIOB->PIO_CODR = SOLENOID_PIN;

    pulse_start = now;
    last_shot = now;

    solenoid_active = true;
    has_fired = true;
}


// Update shooting state
void solenoid_update(void)
{
    if (!solenoid_active)
    {
        return;
    }

    // End pulse after 40 ms
    if (time_now() - pulse_start >= msecs(SOLENOID_PULSE_MS))
    {
        // Deactivate solenoid
        PIOB->PIO_SODR = SOLENOID_PIN;

        solenoid_active = false;
    }
}