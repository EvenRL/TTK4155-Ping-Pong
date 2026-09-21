#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

void SPI_MasterInit(void);
uint8_t SPI_MasterTransmit(uint8_t data);
void SPI_SlaveInit(void);
void oled_select(void);
void oled_deselect(void);

#endif