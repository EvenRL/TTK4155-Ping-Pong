/*
 * GccApplication2.c
 *
 * Created: 14.09.2026 10:19:00
 * Author : freholt
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "Node1/usart.h"
#include "sram.h"
#include "joysticktouch.h"


// =========================================================
//                 TEST PROGRAM - LAB 1
// =========================================================
/*
int main(void)
{
	uint8_t received_character;

	usart_init(9600);
	usart_init_printf();
	
	sei();
	
	printf("\n\nATmegah162 starting...\n");
	
	xmem_init();
	SRAM_test();
	decoder_test();

	while (1)
	{
		// Check whether the receive buffer contains data.
		while (usart_rx(& received_character))
		{
			// Echo received character back to PuTTY.
			usart_tx(received_character);
		}
	}

	return 0;
} */

int main(void)
{

	usart_init(9600);
	usart_init_printf();
	
	sei();
	
	xmem_init();
	adc_init();
	
	uint8_t adc_values[4];
	
	printf("\n\nATmegah162 starting...\n");

	while (1)
	{
		adc_read(adc_values);
		printf("AIN0: %3u AIN1: %3u AIN2: %3u AIN3: %3u\n",
		adc_values[0],
		adc_values[1],
		adc_values[2],
		adc_values[3]);
		_delay_ms(200);
		
	}

	return 0;
} 




