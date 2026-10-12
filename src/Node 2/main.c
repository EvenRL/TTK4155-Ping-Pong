#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"

#define FCPU 84000000 // CPU frequency in Hz

int main()
{
    SystemInit();

    uart_init(FCPU, 115200); // Initialize UART with baud rate 115200

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    PMC->PMC_PCER0 = 1u << ID_PIOB;  // Enable PIOB clock

    PIOB->PIO_PER = PIO_PB25;        // Assign PB25 to PIO
    PIOB->PIO_OER = PIO_PB25;        // Configure as output

    while (1)
    {
        PIOB->PIO_SODR = PIO_PB25;   // High
        for (volatile uint32_t i = 0; i < 1000000; i++) {}

        PIOB->PIO_CODR = PIO_PB25;   // Low
        for (volatile uint32_t i = 0; i < 2000000; i++) {}

        printf("Hello, World!\n");
    }
    
}