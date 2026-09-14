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

#include "usart.h"


// =========================================================
//                 TEST PROGRAM - LAB 1
// =========================================================

int main(void)
{
	uint8_t received_character;

	usart_init(9600);
	usart_init_printf();
	
	printf("\n\nATmega162 starting...\n");
	printf("\n\nTest123456\n");
	
	sei();

	//usart_puts("USART0 test ready!\r\n");
	//usart_puts("Write something in PuTTY:\r\n");

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
}




