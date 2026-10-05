#include "SPI.h"

#define OLED_SS_DDR  DDRB
#define OLED_SS_PORT PORTB
#define OLED_SS_PIN  PB3

#define MCP2515_SS_DDR  DDRB
#define MCP2515_SS_PORT PORTB
#define MCP2515_SS_PIN  PB2

// =========================================================
//                          SPI MASTER
// =========================================================

void SPI_MasterInit(void)
{
    DDRB |= (1 << OLED_SS_PIN) // OLED CS/SS
          | (1 << MCP2515_SS_PIN) // MCP2515 CS/SS
          | (1 << DDB4) // SS
          | (1 << DDB5) // MOSI
          | (1 << DDB7); // SCK

    DDRB &= ~(1 << DDB6); // MISO

    oled_deselect();
    mcp2515_deselect();
    io_deselect();

    SPCR = (1 << SPE) // Enable SPI
         | (1 << MSTR) // Set as Master
         | (1 << SPR0); // Set clock rate


}

uint8_t SPI_MasterTransmit(uint8_t data)
{
    SPDR = data;

    while (!(SPSR & (1 << SPIF)))
    {
    }

    return SPDR;
}

uint8_t SPI_MasterReceive(void)
{
    return SPI_MasterTransmit(0x00);
}


// =========================================================
//                       OLED SELECT
// =========================================================

void oled_select(void)
{
    mcp2515_deselect();
    OLED_SS_PORT &= ~(1 << OLED_SS_PIN);
}

void oled_deselect(void)
{
    OLED_SS_PORT |= (1 << OLED_SS_PIN);
}

// =========================================================
//                       MCP2515 SELECT
// =========================================================

void mcp2515_select(void)
{
    oled_deselect();
    MCP2515_SS_PORT &= ~(1 << MCP2515_SS_PIN);
}

void mcp2515_deselect(void)
{
    MCP2515_SS_PORT |= (1 << MCP2515_SS_PIN);

}
// =========================================================
//                     IO BOARD SELECT
// =========================================================

void io_select(void)
{
    PORTB &= ~(1 << PB4);
}

void io_deselect(void)
{
    PORTB |= (1 << PB4);
}