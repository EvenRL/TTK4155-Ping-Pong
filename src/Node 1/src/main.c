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
	IO_ShowGoals(0); // Slukk alle goal-LEDs ved oppstart
	OLED_Init();
	OLED_Clear();
	joystick_button_init();
	
	if (!can_init(MCP_MODE_NORMAL)) {
		printf("Failed to initialize CAN controller.\n");
	}

	menu_init();
	
	uint32_t rx_id = 0;
	uint8_t rx_length = 0;
	uint8_t data[8] = {0};

	while (1)
	{

		adc_read(adc_values);
		io_buttons_t buttons = IO_ReadButtons();

		bool shoot_pressed = (buttons.right & (1 << 0)) != 0;

		menu_update(adc_values);

		joystick_position_t joystick =
			joystick_get_position(adc_values);

		touch_position_t touch =
			touch_get_position(adc_values);

		int8_t servo_control =
			-(uint8_t)(((int32_t)touch.x * 200) / 255 - 100);

		// CAN transmission: 

		uint8_t message[3] = {
			(uint8_t)joystick.x,
			(uint8_t)servo_control,
			shoot_pressed ? 1 : 0
		};

		static uint32_t can_sent = 0;
		static uint32_t can_failed = 0;

		if (can_send_message(0x03FC0000, message, 3))
		{
			can_sent++;
		}
		else
		{
			can_failed++;
		}

		// Debug output every 25 iterations
		static uint8_t debug_counter = 0;

		if (++debug_counter >= 25)
		{
			debug_counter = 0;

			printf("CAN sent: %lu, failed: %lu, X: %d, Y: %d, SR1: %d\r\n",
				(unsigned long)can_sent,
				(unsigned long)can_failed,
				joystick.x,
				servo_control,
				shoot_pressed);
		}

		// CAN reception

		while (can_take_rx_pending())
		{
			while (can_receive_message(&rx_id, data, &rx_length))
			{
				if (rx_id == 0x00040000UL && rx_length == 1)
				{
					IO_ShowGoals(data[0]);

				}

				printf("Received ID: 0x%03lX data: ", rx_id);

				for (uint8_t i = 0; i < rx_length; i++)
				{
					printf("%d ", data[i]);
				}

				printf("\n");
			}
		}

		_delay_ms(20);
	}
} 


