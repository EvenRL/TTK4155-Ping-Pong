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

#include "io.h"
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

	bool previous_r6 = false;
	bool led_on = false;
	
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
	IO_SetLED(0, false);

	// Dersom CAN ikke initialiseres, vil ikke programmet fortsette.
	// Bare for å sikre at vi ikke prøver å sende meldinger når CAN ikke er initialisert.
	if (!can_init(MCP_MODE_NORMAL)) {
		printf("Failed to initialize CAN controller.\n");
	}

	menu_init();
	
	uint8_t message[8] = {9, 9, 9, 9, 9, 9, 9, 9};

	uint32_t rx_id = 0;
	uint8_t rx_length = 0;
	uint8_t data[8] = {0};

	while (1)
{
    // Read ADC once
    adc_read(adc_values);

	bool r6 = IO_R6_Pressed();

	if (r6 && !previous_r6)
	{
		led_on = !led_on;

		IO_SetLED(0, led_on);
	}

	previous_r6 = r6;

		menu_update(adc_values);
		
		// Get joystick position
		joystick_position_t joystick = joystick_get_position(adc_values);

		// Create CAN message with joystick position
		uint8_t message[2] = {
			(uint8_t)joystick.x,
			(uint8_t)joystick.y
		};

		// Send joystick position over CAN
		if (can_send_message(0x03FC0000, message, 2))
		{
			printf("Joystick sent: X=%d, Y=%d\n", joystick.x, joystick.y);
		}
		else
		{
			printf("CAN send failed\n");
		}

		while (can_take_rx_pending())
		{
			while (can_receive_message(&rx_id, data, &rx_length))
			{
				printf("Received ID: 0x%03lX data: ", rx_id);

				for (uint8_t i = 0; i < rx_length; i++)
				{
					printf("%d ", data[i]);
				}

				printf("\n");
			}
		}

		_delay_ms(100);
	}
} 


