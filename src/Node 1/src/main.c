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

	menu_init();


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

    // Calculate everything from the same ADC reading
    // joystick_position_t joystick =
    //     joystick_get_position(adc_values);

    // joystick_direction_t direction =
    //     joystick_get_direction(joystick);

    // touch_position_t touch =
    //     touch_get_position(adc_values);

    // Update OLED/menu
    menu_update(adc_values);

    // Display values in PuTTY
    //display_joystick(joystick, direction, touch);

    _delay_ms(100);
}
} 



