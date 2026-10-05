#include "io.h"
#include "SPI.h"

#define IO_BUTTON_COMMAND  0x04
#define IO_LED_COMMAND     0x05

#define R6_MASK            (1 << 5)


// Read all buttons from IO board
io_buttons_t IO_ReadButtons(void)
{
    io_buttons_t buttons;

    io_select();

    // Command 0x04 = read buttons
    SPI_MasterTransmit(IO_BUTTON_COMMAND);

    // Response is three bytes:
    // right, left, nav
    buttons.right = SPI_MasterTransmit(0x00);
    buttons.left  = SPI_MasterTransmit(0x00);
    buttons.nav   = SPI_MasterTransmit(0x00);

    io_deselect();

    return buttons;
}


// Check specifically whether R6 is pressed
bool IO_R6_Pressed(void)
{
    io_buttons_t buttons = IO_ReadButtons();

    return (buttons.right & R6_MASK) != 0;
}


// Turn one of the six IO-board LEDs on/off
void IO_SetLED(uint8_t led, bool on)
{
    if (led > 5)
    {
        return;
    }

    io_select();

    // Command 0x05 = LED on/off
    SPI_MasterTransmit(IO_LED_COMMAND);

    // LED number: 0-5
    SPI_MasterTransmit(led);

    // 0 = off, anything else = on
    SPI_MasterTransmit(on ? 1 : 0);

    io_deselect();
}