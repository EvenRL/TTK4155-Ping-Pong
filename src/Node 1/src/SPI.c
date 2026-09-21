
#include "SPI.h"

// =========================================================
//                          SPI MASTER
// =========================================================

void SPI_MasterInit(void){
    // Set SS, MOSI, SCK, D/C to output
    DDRB |= (1 << DDB4) | (1 << DDB5) | (1 << DDB7) | (1 << DDB1);
    // Set MISO to input
    DDRB &= ~(1 << DDB6);
    // Set PB4 as output (SS)
    PORTB |= (1 << PB4);
    // Enable SPI, set ATmeage as Master, set clock rate = F_CPU/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

// uint instead of void to be able to return values if needed later
uint8_t SPI_MasterTransmit(uint8_t data){
    // Set D/C pin low to indicate command byte
    PORTB &= ~(1 << PB1); // Fix later!!!!
    // Put the byte we want to transmit in the SPI Data Register
    SPDR = data;

    // When data arrive, the SPI Interrupt Flag is set
    // Waiting until SPIF is set 
    while(!(SPSR & (1 << SPIF)))
        ;
    
        return SPDR;
}


// =========================================================
//                          SPI SLAVE(R)
// =========================================================

// defines for oled slave select data direction register, port and pin
// in case we need to switch the pin later in the lab
#define OLED_SS_DDR DDRB
#define OLED_SS_PORT PORTB
#define OLED_SS_PIN PB3

void SPI_SlaveInit(void){
    // Set MISO output, all others input
    OLED_SS_DDR |= (1 << OLED_SS_PIN);
    // set HIGH = not selected by default
    OLED_SS_PORT |= (1 << OLED_SS_PIN);
}

void oled_select(void){
    // LOW = select slave
    OLED_SS_PORT &= ~(1 << OLED_SS_PIN);
}

void oled_deselect(void){
    // HIGH = deselect slave
    OLED_SS_PORT |= (1 << OLED_SS_PIN);
}
