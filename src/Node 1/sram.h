
#ifndef SRAM_H
#define SRAM_H

#define F_CPU 4915200UL

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

void xmem_init(void);
void SRAM_test(void);
void decoder_test(void);

#endif