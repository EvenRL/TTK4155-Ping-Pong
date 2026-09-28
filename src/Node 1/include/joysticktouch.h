
#ifndef JOYSTICKTOUCH_H
#define JOYSTICKTOUCH_H
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int8_t x;
	int8_t y;
	} joystick_position_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} touch_position_t;
	 
typedef enum {
	NEUTRAL,
	LEFT,
	RIGHT,
	UP,
	DOWN} joystick_direction_t;
	
void adc_init(void);
void adc_read(uint8_t values[4]);

void joystick_find_center(void);
void joystick_calibrate(void);

joystick_position_t joystick_get_position(uint8_t adc_values[4]);
joystick_direction_t joystick_get_direction(joystick_position_t joystick);
touch_position_t touch_get_position(uint8_t adc_values[4]);

void joystick_button_init(void);
bool joystick_button_pressed(void);

void display_joystick(
	joystick_position_t joystick, 
	joystick_direction_t direction, 
	touch_position_t touch);
	

#endif
