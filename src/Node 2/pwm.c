#include "sam.h"
#include "pwm.h"

#define SERVO_PWM_CHANNEL 1u
#define SERVO_PWM_PIN PIO_PB13
#define SERVO_PWM_PERIOD_US 20000u

void pwm_init(void) {
    // Enable peripheral clocks
    PMC->PMC_PCER0 = (1u << ID_PIOB);
    PMC->PMC_PCER1 = (1u << (ID_PWM - 32));

    // Disable PWM channel during configuration
    PWM->PWM_DIS = (1u << SERVO_PWM_CHANNEL);

    // Configure PC18 as peripheral B (PWMH6)
    PIOC->PIO_ABSR |= SERVO_PWM_PIN;
    PIOC->PIO_PDR = SERVO_PWM_PIN;

    // Configure PWM clock A to 1 MHz
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);

    // Configure PWM channel 6
    // Left aligned, active high, clock A
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CMR =
        PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set period to 20 ms
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CPRD =
        SERVO_PWM_PERIOD_US;

    // Set initial pulse width to 1.5 ms
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTY = 1500;

    // Enable PWM channel
    PWM->PWM_ENA = (1u << SERVO_PWM_CHANNEL);
}

void pwm_set_pulse_width(uint16_t pulse_us) {
    if (pulse_us < 900) pulse_us = 900; // Minimum pulse width
    if (pulse_us > 2100) pulse_us = 2100; // Maximum pulse width

    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD = pulse_us;
}
