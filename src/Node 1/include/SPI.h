#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdint.h>

void SPI_MasterInit(void);
uint8_t SPI_MasterTransmit(uint8_t data);

void oled_select(void);
void oled_deselect(void);

#endif