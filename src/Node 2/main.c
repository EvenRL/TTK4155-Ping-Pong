#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"

#define FCPU 84000000 // CPU frequency in Hz

int main()
{
    SystemInit();

    uart_init(FCPU, 115200); // Initialize UART with baud rate 115200

    can_init((CanInit){.phase2=3, .propag=4, .phase1=3, .sjw=0, .brp=11, .smp=0}, 0); // Initialize CAN

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

        printf("Hello, World!\r\n");

        CanMsg msg = (CanMsg){.id=0xFF, .length=8, .byte={1,2,3,4,5,6,7,8}};
        can_tx(msg);
        printf("Sent message:\n");
        for (volatile uint32_t i = 0; i < 2000000; i++) {}

        CanMsg rx_msg;

        if (can_rx(&rx_msg))
        {  
            if (rx_msg.id == 0xFF && rx_msg.length == 2)
            {
                int8_t x = (int8_t)rx_msg.byte[0];
                int8_t y = (int8_t)rx_msg.byte[1];

                printf("Joystick X: %d, Y: %d\n", x, y);
            }
        }

        // printf("Before CAN TX\n");

        // CanMsg msg = (CanMsg){
        //     .id = 0xFF,
        //     .length = 8,
        //     .byte = {1,2,3,4,5,6,7,8}
        // };

        // can_tx(msg);

        // printf("After CAN TX\n");

        // for (volatile uint32_t i = 0; i < 2000000; i++) {}
        }
    
}