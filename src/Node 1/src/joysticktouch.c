
#define F_CPU 4915200UL

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include "joysticktouch.h"

#define ADC_ADDRESS 0x1000
#define JOY_X_CHANNEL 0
#define JOY_Y_CHANNEL 1
#define TOUCH_X_CHANNEL 2
#define TOUCH_Y_CHANNEL 3
#define DEADZONE 3
static uint8_t x_min;
static uint8_t x_center;
static uint8_t x_max;

static uint8_t y_min;
static uint8_t y_center;
static uint8_t y_max;

void joystick_find_center(void){
	uint8_t adc_values[4];
	uint16_t x_sum = 0;
	uint16_t y_sum = 0;
	
	for (uint8_t i = 0; i < 50; i++)
	{
		adc_read(adc_values);
		x_sum += adc_values[JOY_X_CHANNEL];
		y_sum += adc_values[JOY_Y_CHANNEL];
		_delay_ms(10);
	}
	
	x_center = x_sum / 50;
	y_center = y_sum / 50;
}

void joystick_calibrate(void)
{
	uint8_t adc_values[4];
	x_min = 255;
	x_max = 0;
	
	y_min = 255;
	y_max = 0;
	
	for (uint16_t i = 0; i < 500; i++)
	{
		adc_read(adc_values);
		uint8_t x = adc_values[JOY_X_CHANNEL];
		uint8_t y = adc_values[JOY_Y_CHANNEL];
		
		if (x < x_min)
			x_min = x;
		
		if (x > x_max)
			x_max = x;
		
		if (y < y_min)
		y_min = y;
		
		if (y > y_max)
		y_max = y;
		
		_delay_ms(10);

		
	}
	
}

static int8_t joystick_map(
	uint8_t value, 
	uint8_t min,
	uint8_t center,
	uint8_t max
) {
	int16_t result;
	if (value >= center - DEADZONE && value <= center + DEADZONE)
	{
		return 0;
	}
	
	if (value > center) {
		if (max <= center)
		return 0;
		
		result = 
			((int16_t)(value - center) * 100) / (max - center);
	}
	else
	{
		if (center <= min)
			return 0;
			
		result =
		-((int16_t)(center - value) * 100) / (center - min);
	}
	
	if (result > 100)
		result = 100;
	
	if (result < -100)
		result = -100;
		
	
		
	return (int8_t)result;	
}

joystick_position_t joystick_get_position(uint8_t adc_values[4])
{
	joystick_position_t position;

	position.x = joystick_map(
	adc_values[JOY_X_CHANNEL],
	x_min,
	x_center,
	x_max);
		
	position.y = joystick_map(
	adc_values[JOY_Y_CHANNEL],
	y_min,
	y_center,
	y_max);
	
	return position;
}

#define DIRECTION_THRESHOLD 30

joystick_direction_t joystick_get_direction(
    joystick_position_t joystick)
{
    if (joystick.x < -DIRECTION_THRESHOLD)
        return LEFT;

    if (joystick.x > DIRECTION_THRESHOLD)
        return RIGHT;

    if (joystick.y > DIRECTION_THRESHOLD)
        return UP;

    if (joystick.y < -DIRECTION_THRESHOLD)
        return DOWN;

    return NEUTRAL;
}

touch_position_t touch_get_position(uint8_t adc_values[4])
{
    touch_position_t position;

    position.x = adc_values[TOUCH_X_CHANNEL];
    position.y = adc_values[TOUCH_Y_CHANNEL];

    return position;
}

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

void joystick_button_init(void)
{
	// Set PD3 as input with pull-up resistor
	DDRD	&= ~(1 << PD3);
	PORTD	|= (1 << PD3);
}

bool joystick_button_pressed(void)
{
	// Return true if button is pressed (active low)
	return !(PIND & (1 << PD3));
}

void display_joystick(
	joystick_position_t joystick, 
	joystick_direction_t direction, 
	touch_position_t touch)
{
		printf("\033[H");   // cursor back to top-left

    	printf("====================================\r\n");
    	printf("        JOYSTICK / TOUCHPAD\r\n");
    	printf("====================================\r\n");

    	printf("Joystick X : %4d %%\r\n", joystick.x);
    	printf("Joystick Y : %4d %%\r\n", joystick.y);

    	printf("Direction  : ");

    	switch (direction)
		{
			case LEFT:
				printf("LEFT     ");
				break;

			case RIGHT:
				printf("RIGHT    ");
				break;

			case UP:
				printf("UP       ");
				break;

			case DOWN:
				printf("DOWN     ");
				break;

			case NEUTRAL:
				printf("NEUTRAL  ");
				break;
		}

		printf("\r\n");

		printf("Touch X    : %3u\r\n", touch.x);
		printf("Touch Y    : %3u\r\n", touch.y);

		printf("====================================\r\n");
}


