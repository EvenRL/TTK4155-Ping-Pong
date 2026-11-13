#include "menu.h"
#include "oled.h"
#include "joysticktouch.h"
#include "graphics.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MENU_ITEM_COUNT 3
#define JOYSTICK_REDRAW_THRESHOLD 2

typedef enum {
    page_menu,
    page_joystick,
    page_wasted,
    page_image
} menu_page_t;


// State:

static menu_page_t current_page = page_menu;

static uint8_t menu_index = 0;

static joystick_direction_t previous_direction = NEUTRAL;

static joystick_position_t previous_joystick = {
    .x = 0,
    .y = 0
};

static bool previous_button = false;


// Menu items:

static const char *menu_items[MENU_ITEM_COUNT] = {
    "Joystick",
    "WASTED",
    "Image"
};


// Helper functions:

static const char *direction_to_string(
    joystick_direction_t direction)
{
    switch (direction)
    {
        case LEFT:
            return "LEFT";

        case RIGHT:
            return "RIGHT";

        case UP:
            return "UP";

        case DOWN:
            return "DOWN";

        default:
            return "NEUTRAL";
    }
}


// Main menu drawing function:

static void menu_draw_main(void)
{
    OLED_Clear();

    // Normal display mode (not inverted)
    OLED_WriteCommand(0xA6);

    OLED_Pos(0, 2);
    OLED_Print("=== MAIN MENU ===");


    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++)
    {
        OLED_Pos(2 + i, 2);

        if (i == menu_index)
        {
            OLED_Print("> ");
        }
        else
        {
            OLED_Print("  ");
        }

        OLED_Print(menu_items[i]);
    }


    OLED_Pos(7, 0);
    OLED_Print("UP/DOWN  BTN=SELECT");
}

// Joystick page

static void menu_draw_joystick_static(void)
{
    OLED_Clear();

    OLED_Pos(0, 1);
    OLED_Print("=== JOYSTICK ===");

    OLED_Pos(6, 0);
    OLED_Print("----------------");

    OLED_Pos(7, 0);
    OLED_Print("BTN = BACK");
}


static void menu_draw_joystick_values(
    joystick_position_t joystick,
    joystick_direction_t direction)
{
    char buffer[20];


    OLED_Clear_Line(2);
    OLED_Pos(2, 1);

    snprintf(
        buffer,
        sizeof(buffer),
        "X: %4d%%",
        joystick.x
    );

    OLED_Print(buffer);


    OLED_Clear_Line(3);
    OLED_Pos(3, 1);

    snprintf(
        buffer,
        sizeof(buffer),
        "Y: %4d%%",
        joystick.y
    );

    OLED_Print(buffer);


    OLED_Clear_Line(4);
    OLED_Pos(4, 1);

    snprintf(
        buffer,
        sizeof(buffer),
        "DIR: %-7s",
        direction_to_string(direction)
    );

    OLED_Print(buffer);
}


static void menu_open_joystick(
    joystick_position_t joystick,
    joystick_direction_t direction)
{
    menu_draw_joystick_static();

    menu_draw_joystick_values(
        joystick,
        direction
    );

    previous_joystick = joystick;
}


// WASTED page

static void menu_draw_wasted_page(void)
{
    OLED_Clear();

    for (uint8_t page = 0; page < WASTED_PAGES; page++)
    {
        OLED_GotoLine(2 + page);
        OLED_GotoColumn(11);

        for (uint8_t column = 0;
             column < WASTED_WIDTH;
             column++)
        {
            uint8_t data =
                pgm_read_byte(
                    &wasted_bitmap[page][column]
                );

            OLED_WriteData(data);
        }
    }

    OLED_Pos(7, 0);
    OLED_Print("BTN = BACK");
}


// Image page

static void menu_draw_image_page(void)
{
    static const uint8_t animation_sequence[] = {
        0, 1, 2, 3, 4
    };

    const uint8_t sequence_length =
        sizeof(animation_sequence) /
        sizeof(animation_sequence[0]);

    // wait for button release before starting animation
    while (joystick_button_pressed())
    {
        _delay_ms(10);
    }

    // Keep displaying frames until the BACK button is pressed
    while (!joystick_button_pressed())
    {
        for (uint8_t i = 0; i < sequence_length; i++)
        {
            uint8_t frame = animation_sequence[i];

            for (uint8_t page = 0; page < 8; page++)
            {
                OLED_GotoLine(page);
                OLED_GotoColumn(0);

                for (uint8_t column = 0; column < 128; column++)
                {
                    uint16_t index =
                        (uint16_t)page * 128 + column;

                    uint8_t data =
                        pgm_read_byte(
                            &ping_pong_frames[frame][index]
                        );

                    OLED_WriteData(data);
                }
            }

            _delay_ms(700);

            if (joystick_button_pressed())
            {
                break;
            }
        }
    }

    // wait for button release before returning to menu
    while (joystick_button_pressed())
    {
        _delay_ms(10);
    }

    current_page = page_menu;
    menu_draw_main();
}


// page selection

static void menu_open_selected_page(
    joystick_position_t joystick,
    joystick_direction_t direction)
{
    switch (menu_index)
    {
        case 0:
            current_page = page_joystick;

            menu_open_joystick(
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


// Init

void menu_init(void)
{
    current_page = page_menu;

    menu_index = 0;

    previous_direction = NEUTRAL;

    previous_joystick.x = 0;
    previous_joystick.y = 0;

    previous_button = false;

    menu_draw_main();
}


// Menu navigation

static void menu_handle_navigation(
    joystick_direction_t direction)
{
    // If the previous direction was not NEUTRAL, we ignore the current direction
    if (previous_direction != NEUTRAL)
    {
        return;
    }


    if (direction == UP)
    {
        if (menu_index > 0)
        {
            menu_index--;

            menu_draw_main();
        }
    }

    else if (direction == DOWN)
    {
        if (menu_index < MENU_ITEM_COUNT - 1)
        {
            menu_index++;

            menu_draw_main();
        }
    }
}


// joystick page update

static void menu_update_joystick_page(
    joystick_position_t joystick,
    joystick_direction_t direction)
{
    int16_t dx =
        abs(
            joystick.x -
            previous_joystick.x
        );

    int16_t dy =
        abs(
            joystick.y -
            previous_joystick.y
        );


    bool position_changed =
        dx >= JOYSTICK_REDRAW_THRESHOLD ||
        dy >= JOYSTICK_REDRAW_THRESHOLD;

    bool direction_changed =
        direction != previous_direction;


    if (position_changed || direction_changed)
    {
        menu_draw_joystick_values(
            joystick,
            direction
        );

        previous_joystick = joystick;
    }
}


// Menu update function

void menu_update(uint8_t adc_values[4])
{
    joystick_position_t joystick =
        joystick_get_position(adc_values);

    joystick_direction_t direction =
        joystick_get_direction(joystick);

    bool button =
        joystick_button_pressed();

    bool button_pressed =
        button && !previous_button;


    // Button:

    if (button_pressed)
    {
        if (current_page == page_menu)
        {
            menu_open_selected_page(
                joystick,
                direction
            );
        }

        else
        {
            current_page = page_menu;

            menu_draw_main();
        }


        previous_button = button;
        previous_direction = direction;
        previous_joystick = joystick;

        return;
    }


    // Current page handling:

    switch (current_page)
    {
        case page_menu:

            menu_handle_navigation(
                direction
            );

            break;


        case page_joystick:

            menu_update_joystick_page(
                joystick,
                direction
            );

            break;


        case page_wasted:

            // static page, no updates needed
            break;


        case page_image:

            // static page, no updates needed
            break;


        default:
            current_page = page_menu;

            menu_draw_main();

            break;
    }


    previous_direction = direction;
    previous_button = button;
}