#include "menu.h"
#include "oled.h"
#include "joysticktouch.h"
#include "graphics.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

typedef enum {
    page_menu,
    page_joystick,
    page_wasted,
    page_image
} menu_page_t;


static menu_page_t current_page = page_menu;

static uint8_t menu_index = 0;

static joystick_direction_t previous_direction = NEUTRAL;
static bool previous_button = false;


/* =========================
   MAIN MENU
   ========================= */

static void menu_draw_main(void)
{
    OLED_Clear();
    OLED_WriteCommand(0xA6);

    OLED_Pos(0, 0);
    OLED_Print("MAIN MENU");

    OLED_Pos(2, 0);
    OLED_Print(menu_index == 0 ? "> Joystick" : "  Joystick");

    OLED_Pos(3, 0);
    OLED_Print(menu_index == 1 ? "> WASTED" : "  WASTED");

    OLED_Pos(4, 0);
    OLED_Print(menu_index == 2 ? "> Image" : "  Image");
}


/* =========================
   JOYSTICK PAGE
   ========================= */

static void menu_draw_joystick_page(
    joystick_position_t joystick,
    joystick_direction_t direction)
{
    char buffer[20];

    OLED_Clear_Line(2);
    OLED_Pos(2, 0);

    snprintf(
        buffer,
        sizeof(buffer),
        "X:%4d%% Y:%4d%%",
        joystick.x,
        joystick.y
    );

    OLED_Print(buffer);


    OLED_Clear_Line(3);
    OLED_Pos(3, 0);

    snprintf(
        buffer,
        sizeof(buffer),
        "Dir: %s",
        (direction == LEFT)  ? "LEFT" :
        (direction == RIGHT) ? "RIGHT" :
        (direction == UP)    ? "UP" :
        (direction == DOWN)  ? "DOWN" :
                               "NEUTRAL"
    );

    OLED_Print(buffer);
}


/* =========================
   WASTED PAGE
   ========================= */

static void menu_draw_wasted_page(void)
{
    OLED_Clear();

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
}


/* =========================
   IMAGE PAGE
   ========================= */

static void menu_draw_image_page(void)
{
    OLED_Clear();
    OLED_DrawBitmap(image);
}


/* =========================
   INIT
   ========================= */

void menu_init(void)
{
    current_page = page_menu;
    menu_index = 0;

    previous_direction = NEUTRAL;
    previous_button = false;

    menu_draw_main();
}


/* =========================
   MENU UPDATE
   ========================= */

void menu_update(uint8_t adc_values[4])
{
    joystick_position_t joystick =
        joystick_get_position(adc_values);

    joystick_direction_t direction =
        joystick_get_direction(joystick);

    bool button = joystick_button_pressed();

    bool button_pressed =
        button && !previous_button;


    /* -------------------------
       BUTTON
       ------------------------- */

    if (button_pressed)
    {
        /* We are in the main menu -> enter page */
        if (current_page == page_menu)
        {
            switch (menu_index)
            {
                case 0:
                    current_page = page_joystick;

                    OLED_Clear();

                    menu_draw_joystick_page(
                        joystick,
                        direction
                    );
                    break;


                case 1:
                    current_page = page_wasted;

                    menu_draw_wasted_page();
                    break;


                case 2:
                    current_page = page_image;

                    menu_draw_image_page();
                    break;


                default:
                    break;
            }
        }

        /* We are inside a page -> go back */
        else
        {
            current_page = page_menu;

            menu_draw_main();
        }

        /*
         * Return here so the same button press
         * cannot do anything else this cycle.
         */
        previous_button = button;
        previous_direction = direction;

        return;
    }


    /* -------------------------
       CURRENT PAGE
       ------------------------- */

    switch (current_page)
    {
        case page_menu:

            /*
             * Only react when the joystick
             * changes direction.
             */
            if (direction != previous_direction)
            {
                if (direction == UP && menu_index > 0)
                {
                    menu_index--;
                    menu_draw_main();
                }

                else if (direction == DOWN && menu_index < 2)
                {
                    menu_index++;
                    menu_draw_main();
                }
            }

            break;


        case page_joystick:

            /*
             * Continuously update values.
             */
            menu_draw_joystick_page(
                joystick,
                direction
            );

            break;


        case page_wasted:

            /*
             * Nothing to update.
             * Image was drawn when page opened.
             */
            break;


        case page_image:

            /*
             * Nothing to update.
             * Image was drawn when page opened.
             */
            break;


        default:
            break;
    }


    previous_direction = direction;
    previous_button = button;
}