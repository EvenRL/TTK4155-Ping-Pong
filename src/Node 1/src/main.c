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
#include "sram.h"
#include "joysticktouch.h"
#include "SPI.h"
#include "oled.h"
#include "graphics.h"
#include "menu.h"
#include "can.h"

int main(void)
{

	usart_init(9600);
	usart_init_printf();
	
	sei();
	
	xmem_init();
	adc_init();
	uint8_t adc_values[4];
	printf("\n\nATmegah162 starting...\n");
	printf("Leave joystick in center...\n");
	
	_delay_ms(1000);
	
	joystick_find_center();
	printf("Move joystick all the way around for 5 seconds...\n");
	
	joystick_calibrate();
	
	printf("Calibration complete!\n");
	printf("\033[2J\033[H");
	
	SPI_MasterInit();
	OLED_Init();
	OLED_Clear();
	joystick_button_init();
	can_init(MCP_MODE_LOOPBACK);

	menu_init();
	
	uint8_t message[8] = {9, 9, 9, 9, 9, 9, 9, 9};

	uint32_t rx_id = 0;
	uint8_t rx_length = 0;
	uint8_t data[8] = {0};

	while (1)
	{
		adc_read(adc_values);

		menu_update(adc_values);

		if (can_send_message(0x123, message, 8))
		{
			printf("Sent message\n");
		}
		else
		{
			printf("Send failed\n");
		}

		if (can_receive_message(&rx_id, data, &rx_length))
		{
			printf("Received ID: 0x%03lX data: ", rx_id);

			for (uint8_t i = 0; i < rx_length; i++)
			{
				printf("%d ", data[i]);
			}

			printf("\n");
		}
		else
		{
			printf("No message received\n");
		}

		_delay_ms(100);
	}
} 



