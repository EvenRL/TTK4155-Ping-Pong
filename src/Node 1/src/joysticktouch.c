
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

joystick_position_t joystick_get_position(void)
{
	uint8_t adc_values[4];
	joystick_position_t position;
	adc_read(adc_values);
	
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

#define DIRECTION_THRESHHOLD 30
joystick_direction_t joystick_get_direction(void) {
	
	joystick_position_t joystick = joystick_get_position();
	if (joystick.x < -DIRECTION_THRESHHOLD)
		return LEFT;
	
	if (joystick.x > DIRECTION_THRESHHOLD)
	return RIGHT;
	
	if (joystick.y > DIRECTION_THRESHHOLD)
	return UP;
	
	if (joystick.y < -DIRECTION_THRESHHOLD)
	return DOWN;
	
	return NEUTRAL;
	
}

joystick_position_t touch_get_position(void)
{
	uint8_t adc_values[4];
	joystick_position_t position;
	adc_read(adc_values);
	
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

