#include "oled.h"
#include "SPI.h"
#include "graphics.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#define OLED_DC_PORT PORTB
#define OLED_DC_PIN  PB1

void OLED_Init(void)
{
    DDRB |= (1 << DDB1); // D/C

    PORTB |= (1 << PB3); // deselect OLED

    OLED_WriteCommand(0xA1); // Segment remap
    OLED_WriteCommand(0xC8); // Reverse COM output scan direction

    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xAF);
}

void OLED_Reset(void)
{
    // Reset sequence for the OLED
    OLED_WriteCommand(0xAE); // Display off
    OLED_Clear();
    OLED_Home();
    OLED_WriteCommand(0xAF); // Display on
}

void OLED_WriteCommand(uint8_t command)
{
    // D/C low = command
    OLED_DC_PORT &= ~(1 << OLED_DC_PIN);

    oled_select();

    SPI_MasterTransmit(command);

    oled_deselect();
}

void OLED_WriteData(uint8_t data)
{
    // D/C high = data
    OLED_DC_PORT |= (1 << OLED_DC_PIN);

    oled_select();

    SPI_MasterTransmit(data);

    oled_deselect();
}

void OLED_GotoLine(uint8_t line)
{
    if (line > 7)
        return;

    OLED_WriteCommand(0xB0 | line);
}

void OLED_GotoColumn(uint8_t column)
{
    if (column > 127)
        return;

    OLED_WriteCommand(0x00 | (column & 0x0F));
    OLED_WriteCommand(0x10 | (column >> 4));
}

void OLED_Home(void)
{
    OLED_GotoLine(0);
    OLED_GotoColumn(0);
}

void OLED_Pos(uint8_t line, uint8_t column)
{
    OLED_GotoLine(line);
    OLED_GotoColumn(column);
}

void OLED_Clear_Line(uint8_t line)
{
    if (line > 7)
        return;

    OLED_GotoLine(line);
    OLED_GotoColumn(0);

    for (uint8_t column = 0; column < 128; column++)
    {
        OLED_WriteData(0x00);
    }
    OLED_GotoColumn(0); // Return to the beginning of the line
}

void OLED_Print_Char(char c)
{
    // Each character is 5 pixels wide, plus 1 pixel space
    for (uint8_t i = 0; i < 5; i++)
    {
        uint8_t data = pgm_read_byte(&font5[c - 32][i]);
        OLED_WriteData(data);
    }

    // Add a space between characters
    OLED_WriteData(0x00);
}

void OLED_Print(const char *text)
{
    while (*text)
    {
        OLED_Print_Char(*text);
        text++;
    }
}

void OLED_Clear(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_GotoLine(page);
        OLED_GotoColumn(0);

        for (uint8_t column = 0; column < 128; column++)
        {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_DrawBitmap(const uint8_t *bitmap)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_GotoLine(page);
        OLED_GotoColumn(0);

        for (uint8_t column = 0; column < 128; column++)
        {
            uint8_t data = pgm_read_byte(
                &bitmap[page * 128 + column]
            );

            OLED_WriteData(data);
        }
    }
}