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
	// OLED_WriteCommand(0xA5);
	// OLED_WriteCommand(0xAF);
	
	//OLED_DrawBitmap(image);

	// WASTED is 3 pages high and 105 pixels wide
    // Start at page 2 and column 11 to roughly center it
    for (uint8_t page = 0; page < WASTED_PAGES; page++)
    {
        OLED_GotoLine(2 + page);
        OLED_GotoColumn(11);

        for (uint8_t column = 0; column < WASTED_WIDTH; column++)
        {
            uint8_t data =
                pgm_read_byte(&wasted_bitmap[page][column]);

            OLED_WriteData(data);
        }
    }
	

	while (1)
	{
		// adc_read(adc_values);
		
		// joystick_position_t joystick = 
		// 	joystick_get_position();
			
		// joystick_direction_t direction = joystick_get_direction();
		
		// printf(
		// "Joy X: %4d%%  Y: %4d%%  ",
		// joystick.x,
		// joystick.y);
		
		// _delay_ms(100);
		
		
		// switch (direction)
		// {
		// 	case LEFT:
		// 	printf("LEFT  ");
		// 	break;

		// 	case RIGHT:
		// 	printf("RIGHT  ");
		// 	break;

		// 	case UP:
		// 	printf("UP  ");
		// 	break;

		// 	case DOWN:
		// 	printf("DOWN  ");
		// 	break;

		// 	case NEUTRAL:
		// 	printf("NEUTRAL  ");
		// 	break;
		// }
		
		// touch_position_t touch = touch_get_position();

		// printf(
		// 	"Touch-pad X: %3u  Y: %3u\r\n",
		// 	touch.x,
		// 	touch.y
		// ); 
		
		// //OLED_WriteCommand(0xAF);
		// _delay_ms(100);

		adc_read(adc_values);

    	joystick_position_t joystick = joystick_get_position();
    	joystick_direction_t direction = joystick_get_direction();
    	touch_position_t touch = touch_get_position();

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

    _delay_ms(100);
	}
} 



