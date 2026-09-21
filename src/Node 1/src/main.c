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
	
	SPI_MasterInit();
	SPI_SlaveInit();
	oled_select();
	SPI_MasterTransmit(0xaf);
	oled_deselect();
	
	while (1)
	{
		adc_read(adc_values);
		
		joystick_position_t joystick = 
			joystick_get_position();
			
		joystick_direction_t direction = joystick_get_direction();
		
		printf(
		"Joy X: %4d%%  Y: %4d%%  ",
		joystick.x,
		joystick.y);
		
		_delay_ms(100);
		
		
		switch (direction)
		{
			case LEFT:
			printf("LEFT  ");
			break;

			case RIGHT:
			printf("RIGHT  ");
			break;

			case UP:
			printf("UP  ");
			break;

			case DOWN:
			printf("DOWN  ");
			break;

			case NEUTRAL:
			printf("NEUTRAL  ");
			break;
		}
		
		joystick_position_t touch = touch_get_position();
		printf(
		"Touch-pad X: %3u  Y: %3u\n",
		touch.x,
		touch.y);
		_delay_ms(100);
	}
} 



