
#ifndef USART_H
#define USART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

void usart_init(uint32_t baud);

void usart_tx(uint8_t data);
//void usart_puts(const char *str);

bool usart_rx(uint8_t *data);
bool usart_data_available(void);

void usart_init_printf(void);

#endif
