#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "pwm.h"
#include "ir.h"
#include "goal_counter.h"

#define FCPU 84000000 // CPU frequency in Hz

int main()
{
    SystemInit();

    uart_init(FCPU, 115200); // Initialize UART with baud rate 115200
    printf("Hello, World!\r\n");
    can_init((CanInit){.phase2=3, .propag=4, .phase1=3, .sjw=0, .brp=11, .smp=0}, 0); // Initialize CAN
    pwm_init(); // Initialize PWM

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    PMC->PMC_PCER0 = 1u << ID_PIOB;  // Enable PIOB clock

    PIOB->PIO_PER = PIO_PB25;        // Assign PB25 to PIO
    PIOB->PIO_OER = PIO_PB25;        // Configure as output

    pwm_set_control(0);

    printf("Setting pwm pulse width to 1.5 ms\n");
        pwm_set_pulse_width(2100); // Set initial pulse width to 1.5 ms

    printf("Starting IR sensor test!\r\n");
    ir_init(); // Initialize IR sensor
    printf("IR sensor initialized!\r\n");
    uint8_t goal_counter = 0; // Initialize goal counter

    while (1)
    {
        // PIOB->PIO_SODR = PIO_PB25;   // High
        // for (volatile uint32_t i = 0; i < 1000000; i++) {}

        // PIOB->PIO_CODR = PIO_PB25;   // Low
        // for (volatile uint32_t i = 0; i < 2000000; i++) {}



        // printf("Hello, World!\r\n");


        // CanMsg msg = (CanMsg){.id=0xFF, .length=8, .byte={1,2,3,4,5,6,7,8}};
        // can_tx(msg);
        // printf("Sent message:\n");
        // for (volatile uint32_t i = 0; i < 2000000; i++) {}

        CanMsg rx_msg;

        if (can_rx(&rx_msg))
        {  
            if (rx_msg.id == 0xFF && rx_msg.length == 2)
            {
                // Read joystick position
                int8_t x = (int8_t)rx_msg.byte[0];
                int8_t y = (int8_t)rx_msg.byte[1];

                // Convert joystick Y to PWM
                uint16_t pulse_width = pwm_set_control(y);

                // Debug
                printf("X: %d, Y: %d, PWM: %u us\r\n", x, y, pulse_width);
            }
        }

        // IR read test

        uint16_t ir_value = ir_read();
        printf("IR sensor value: %u\r\n", ir_value);

        // Goal counter
        check_goal(ir_value, 1500, &goal_counter);
        printf("Goal counter: %u\r\n", goal_counter);

        // Delay between reading for debugging
        for (volatile uint32_t i = 0; i < 1000000; i++) {}



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