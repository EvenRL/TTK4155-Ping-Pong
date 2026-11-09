#include "sam.h"
#include "encoder.h"

#define ENCODER_A PIO_PC25
#define ENCODER_B PIO_PC26

void encoder_init(void) {
    // Enable peripheral clocks for PIOC
    PMC->PMC_PCER0 |= (1u << ID_PIOC);

    PMC->PMC_PCER1 |= (1u << (ID_TC6 - 32)) | (1u << (ID_TC7 - 32));

    // Configure PC25 and PC26 as peripheral B
    PIOC->PIO_ABSR |= ENCODER_A | ENCODER_B;
    PIOC->PIO_PDR = ENCODER_A | ENCODER_B;

    // Disable timer channels during config
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;

    // Config TC2 channels 0 and 1 in capture mode
    // Select external clock XC0
    TC2->TC_CHANNEL[0].TC_CMR = 
        TC_CMR_TCCLKS_XC0 | 
        TC_CMR_ETRGEDG_RISING | 
        TC_CMR_ABETRG;
    
    TC2->TC_CHANNEL[1].TC_CMR = 
        TC_CMR_TCCLKS_XC0 | 
        TC_CMR_ETRGEDG_RISING | 
        TC_CMR_ABETRG;

    // Enable quadrature decoder in position mode
    TC2->TC_BMR = 
        TC_BMR_QDEN | 
        TC_BMR_POSEN |
        TC_BMR_EDGPHA;

    // Start counters
    TC2->TC_CHANNEL[0].TC_CCR = 
        TC_CCR_CLKEN | TC_CCR_SWTRG;

    TC2->TC_CHANNEL[1].TC_CCR = 
        TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t encoder_get_position(void) {
    // Read the counter value from TC2 channel 0
    return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}