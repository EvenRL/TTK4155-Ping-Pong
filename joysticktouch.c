#define F_CPU 4915200UL

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include "joysticktouch.h"

#define ADC_ADDRESS 0x1000

void adc_init(void)
{
	DDRD |= (1 << PD5);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);
	
	OCR1A = 1;
}

void adc_read(uint8_t values[4])
{
	volatile uint8_t *adc = (volatile uint8_t *)ADC_ADDRESS;
	*adc = 0;
	_delay_us(100);
	values[0] = *adc;
	values[1] = *adc;
	values[2] = *adc;
	values[3] = *adc;
}

