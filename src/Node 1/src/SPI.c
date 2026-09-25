#include "SPI.h"

// =========================================================
//                          SPI MASTER
// =========================================================

void SPI_MasterInit(void)
{
    DDRB |= (1 << DDB4)
          | (1 << DDB5)
          | (1 << DDB7);

    DDRB &= ~(1 << DDB6);

    PORTB |= (1 << PB4);

    SPCR = (1 << SPE)
         | (1 << MSTR)
         | (1 << SPR0);
}

uint8_t SPI_MasterTransmit(uint8_t data)
{
    SPDR = data;

    while (!(SPSR & (1 << SPIF)))
    {
    }

    return SPDR;
}


// =========================================================
//                       OLED SELECT
// =========================================================

#define OLED_SS_DDR  DDRB
#define OLED_SS_PORT PORTB
#define OLED_SS_PIN  PB3

void oled_select(void)
{
    OLED_SS_PORT &= ~(1 << OLED_SS_PIN);
}

void oled_deselect(void)
{
    OLED_SS_PORT |= (1 << OLED_SS_PIN);
}