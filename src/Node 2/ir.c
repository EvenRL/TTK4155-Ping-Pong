#include "sam.h"
#include "ir.h"

#define IR_ADC_CHANNEL 7u // ADC channel for IR sensor

void ir_init(void) {
    // Enable peripheral clock for PIOA and ADC
    PMC->PMC_PCER0 |= (1u << ID_PIOA);
    PMC->PMC_PCER1 |= (1u << (ID_ADC - 32));

    // Disable pull-up on PA16
    PIOA->PIO_PUDR = PIO_PA16;

    // Reset ADC
    ADC->ADC_CR = ADC_CR_SWRST;

    // Configure ADC
    // ADC clock = 84 MHz / (2 * (9 + 1)) = 4.2 MHz
    ADC->ADC_MR =  ADC_MR_PRESCAL(9) | ADC_MR_STARTUP_SUT64 | ADC_MR_TRACKTIM(15) | ADC_MR_TRANSFER(2);

    ADC->ADC_CHER = (1u << IR_ADC_CHANNEL); // Enable channel 7
}

uint16_t ir_read(void) {
    // Start ADC conversion
    ADC->ADC_CR = ADC_CR_START;

    // Wait for conversion to complete
    while (!(ADC->ADC_ISR & (1u << IR_ADC_CHANNEL))) {}

    // Read ADC value
    return (uint16_t)ADC->ADC_CDR[IR_ADC_CHANNEL] & 0xFFF; // Return 12-bit value
}