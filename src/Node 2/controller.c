#include "controller.h"
#include "encoder.h"
#include "sam.h"
#include <stdbool.h>
#include <stdint.h>

// ---------------- MOTOR CONFIGURATION ----------------

#define MOTOR_PWM_PIN       PIO_PB12
#define MOTOR_PHASE_PIN     PIO_PC23
#define MOTOR_PWM_CHANNEL   0
#define MOTOR_PWM_PERIOD    525
#define MOTOR_REVERSED      0

// ---------------- CONTROLLER CONFIGURATION ----------------

#define DT                  0.01f
#define JOYSTICK_DEADZONE   8

#define TARGET_SPEED        4500.0f
#define END_MARGIN          100.0f

#define KP                  6.0f
#define KI                  0.5f
#define MAX_DUTY            0.40f

// ---------------- VARIABLES ----------------

static int8_t joystick_command = 0;

static float target_position = 0.0f;

static int32_t left_limit = 0;
static int32_t right_limit = 0;

static float integral = 0.0f;

static bool limits_valid = false;


// ---------------- HELPER FUNCTIONS ----------------

static float clampf(float x, float low, float high)
{
    if (x < low) return low;
    if (x > high) return high;

    return x;
}


// ---------------- MOTOR CONTROL ----------------

// Positive output increases encoder position.
// Negative output decreases encoder position.

static void motor_set_output(float output)
{
    // Limit motor output
    output = clampf(output, -MAX_DUTY, MAX_DUTY);

    // Determine motor direction
    bool reverse = (output < 0.0f);

#if MOTOR_REVERSED
    reverse = !reverse;
#endif

    // Set PHASE signal
    if (reverse)
    {
        PIOC->PIO_SODR = MOTOR_PHASE_PIN;
    }
    else
    {
        PIOC->PIO_CODR = MOTOR_PHASE_PIN;
    }

    // Convert to positive duty cycle
    if (output < 0.0f)
    {
        output = -output;
    }

    // Update PWM
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CDTYUPD =
        (uint32_t)(output * MOTOR_PWM_PERIOD);
}


// ---------------- INITIALIZATION ----------------

void controller_init(void)
{
    // Enable peripheral clocks
    PMC->PMC_PCER0 |= (1u << ID_PIOB) | (1u << ID_PIOC);
    PMC->PMC_PCER1 |= (1u << (ID_PWM - 32));

    // Configure motor direction pin
    PIOC->PIO_PER = MOTOR_PHASE_PIN;
    PIOC->PIO_OER = MOTOR_PHASE_PIN;
    PIOC->PIO_CODR = MOTOR_PHASE_PIN;

    // Configure motor PWM pin
    PIOB->PIO_PER = MOTOR_PWM_PIN;
    PIOB->PIO_OER = MOTOR_PWM_PIN;
    PIOB->PIO_CODR = MOTOR_PWM_PIN;

    // Disable PWM channel during configuration
    PWM->PWM_DIS = (1u << MOTOR_PWM_CHANNEL);

    // Configure PB12 as peripheral B
    PIOB->PIO_ABSR |= MOTOR_PWM_PIN;
    PIOB->PIO_PDR = MOTOR_PWM_PIN;

    // Configure PWM frequency and polarity
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CMR =
        PWM_CMR_CPRE_MCK_DIV_16 | PWM_CMR_CPOL;

    // 10 kHz PWM
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CPRD =
        MOTOR_PWM_PERIOD;

    // Start with motor stopped
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CDTY = 0;

    // Enable PWM channel
    PWM->PWM_ENA = (1u << MOTOR_PWM_CHANNEL);

    // Initialize controller variables
    joystick_command = 0;
    target_position = (float)encoder_get_position();

    integral = 0.0f;
    limits_valid = false;
}


// ---------------- STOP MOTOR ----------------

void controller_stop(void)
{
    joystick_command = 0;

    integral = 0.0f;

    target_position = (float)encoder_get_position();

    motor_set_output(0.0f);
}


// ---------------- SET POSITION LIMITS ----------------

void controller_set_limits(int32_t left, int32_t right)
{
    controller_stop();

    left_limit = left;
    right_limit = right;

    int32_t distance = left - right;

    if (distance < 0)
    {
        distance = -distance;
    }

    // Check that limits are valid
    limits_valid = ((float)distance > 2.0f * END_MARGIN);
}


// ---------------- JOYSTICK INPUT ----------------

void controller_set_target(int8_t joystick_x)
{
    if (!limits_valid)
    {
        return;
    }

    // Clamp joystick input
    if (joystick_x > 100) joystick_x = 100;
    if (joystick_x < -100) joystick_x = -100;

    // Apply deadzone
    if (joystick_x > -JOYSTICK_DEADZONE &&
        joystick_x < JOYSTICK_DEADZONE)
    {
        joystick_x = 0;
    }

    // Stop motor when joystick is released
    if (joystick_x == 0)
    {
        if (joystick_command != 0)
        {
            controller_stop();
        }

        return;
    }

    // Reset target when joystick changes direction
    if (joystick_command != 0 &&
        ((joystick_x > 0) != (joystick_command > 0)))
    {
        target_position = (float)encoder_get_position();
        integral = 0.0f;
    }

    // Store joystick command
    joystick_command = joystick_x;
}


// ---------------- PI CONTROLLER ----------------

void controller_update(void)
{
    // Stop if controller is inactive
    if (!limits_valid || joystick_command == 0)
    {
        target_position = (float)encoder_get_position();
        integral = 0.0f;

        motor_set_output(0.0f);
        return;
    }

    // Read actual encoder position
    int32_t actual = encoder_get_position();

    // Calculate track limits
    float low = (float)(
        left_limit < right_limit ? left_limit : right_limit
    );

    float high = (float)(
        left_limit > right_limit ? left_limit : right_limit
    );

    float travel = high - low;

    // Determine encoder direction toward left
    float to_left = (left_limit > right_limit) ? 1.0f : -1.0f;

    // Requested movement direction in encoder coordinates
    float desired_direction = -(float)joystick_command * to_left;

    // Prevent driving outward at track boundaries
    if ((actual <= low + END_MARGIN && desired_direction < 0.0f) ||
        (actual >= high - END_MARGIN && desired_direction > 0.0f))
    {
        target_position = (float)actual;
        integral = 0.0f;

        motor_set_output(0.0f);
        return;
    }

    // ---------------- UPDATE TARGET POSITION ----------------

    // Joystick controls how quickly the target moves
    target_position -=
        to_left *
        ((float)joystick_command / 100.0f) *
        TARGET_SPEED * DT;

    // Keep target within track limits
    target_position = clampf(
        target_position,
        low + END_MARGIN,
        high - END_MARGIN
    );

    // ---------------- POSITION FEEDBACK ----------------

    // Calculate normalized position error
    float error =
        (target_position - (float)actual) / travel;

    // ---------------- PI REGULATOR ----------------

    // Proportional term
    float P = KP * error;

    // Proposed integral term
    float proposed_integral =
        integral + KI * DT * error;

    float raw = P + proposed_integral;

    // Anti-windup
    if ((raw >= -MAX_DUTY && raw <= MAX_DUTY) ||
        (raw > MAX_DUTY && error < 0.0f) ||
        (raw < -MAX_DUTY && error > 0.0f))
    {
        integral = clampf(
            proposed_integral,
            -MAX_DUTY,
            MAX_DUTY
        );
    }

    // Calculate controller output
    float output = KP * error + integral;

    // Limit motor power
    output = clampf(output, -MAX_DUTY, MAX_DUTY);

    // ---------------- ENDSTOP PROTECTION ----------------

    // Prevent driving beyond track limits
    if ((actual <= low + END_MARGIN && output < 0.0f) ||
        (actual >= high - END_MARGIN && output > 0.0f))
    {
        output = 0.0f;
        integral = 0.0f;
    }

    // ---------------- APPLY MOTOR OUTPUT ----------------

    motor_set_output(output);
}


// ---------------- DEBUG ----------------

int32_t controller_get_target(void)
{
    return (int32_t)target_position;
}