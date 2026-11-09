#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "pwm.h"
#include "ir.h"
#include "goal_counter.h"
#include "encoder.h"
#include "controller.h"
#include "time.h"

#define FCPU 84000000 // CPU frequency in Hz

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    PMC->PMC_PCER0 = 1u << ID_PIOB;  // Enable PIOB clock

    PIOB->PIO_PER = PIO_PB25;        // Assign PB25 to PIO
    PIOB->PIO_OER = PIO_PB25;        // Configure as output

    uart_init(FCPU, 115200); // Initialize UART with baud rate 115200
    
    can_init((CanInit){.phase2=3, .propag=4, .phase1=3, .sjw=0, .brp=11, .smp=0}, 0); // Initialize CAN
    
    pwm_init(); // Initialize PWM
    pwm_set_control(0);
    printf("Setting pwm pulse width to 1.5 ms\n");
    pwm_set_pulse_width(2100); // Set initial pulse width to 1.5 ms

    ir_init(); // Initialize IR sensor
    uint8_t goal_counter = 0; // Initialize goal counter

    encoder_init(); // Initialize encoder
    printf("Encoder initialized!\r\n");

    controller_init();
    controller_set_limits(5610, 0); // Set encoder limits for the motor
    printf("Motor controller initialized!\r\n");

//     while (1)
//     {
//         int32_t encoder_position = encoder_get_position();
//         printf("Encoder position: %ld\r\n", encoder_position);

//         for (volatile uint32_t i = 0; i < 2000000; i++) {} // Delay for readability

//         CanMsg rx_msg;

//         if (can_rx(&rx_msg))
//         {  
//             if (rx_msg.id == 0xFF && rx_msg.length == 2)
//             {
//                 // Read joystick position
//                 int8_t x = (int8_t)rx_msg.byte[0];
//                 int8_t y = (int8_t)rx_msg.byte[1];

//                 // Convert joystick Y to PWM
//                 uint16_t pulse_width = pwm_set_control(y);

//                 // Debug
//                 printf("X: %d, Y: %d, PWM: %u us\r\n", x, y, pulse_width);
//             }
//         }

//         // IR read test

//         uint16_t ir_value = ir_read();
//         printf("IR sensor value: %u\r\n", ir_value);

//         // Goal counter
//         check_goal(ir_value, 1500, &goal_counter);
//         printf("Goal counter: %u\r\n", goal_counter);

//         // Delay between reading for debugging
//         for (volatile uint32_t i = 0; i < 1000000; i++) {}

//     }
    
// }

    int8_t joystick_x = 0;

    uint64_t last_control = time_now();
    uint64_t last_can_rx = time_now();
    uint64_t last_debug = time_now();

    bool can_active = false;

    while (1)
    {
        // ---------------- CAN RECEIVE ----------------

        CanMsg rx_msg;

        if (can_rx(&rx_msg))
        {
            if (rx_msg.id == 0xFF && rx_msg.length == 2)
            {
                // Read joystick position
                int8_t joystick_y = (int8_t)rx_msg.byte[0];
                int8_t touch_y = (int8_t)rx_msg.byte[1];
                int8_t x = (int8_t)rx_msg.byte[0];
                int8_t y = (int8_t)rx_msg.byte[1];

                // Set desired motor position
                controller_set_target(x);

                // Control servo
                pwm_set_control(y);

                // Update CAN timestamp
                last_can_rx = time_now();
                can_active = true;
            }
        }

        // ---------------- PI CONTROLLER ----------------

        uint64_t now = time_now();

        // Update every 10 ms (100 Hz)
        if (now - last_control >= msecs(10))
        {
            last_control = now;

            // Check CAN connection
            if (can_active &&
                now - last_can_rx <= msecs(200))
            {
                controller_update();
            }
            else
            {
                can_active = false;
                controller_stop();
            }
        }

        // ---------------- IR SENSOR ----------------

        uint16_t ir_value = ir_read();

        // ---------------- GOAL COUNTER ----------------

        check_goal(ir_value, 1500, &goal_counter);

        // ---------------- DEBUG ----------------

        if (now - last_debug >= msecs(250))
        {
            last_debug = now;

            printf("Target: %ld, Actual: %ld\r\n",
                (long)controller_get_target(),
                (long)encoder_get_position());

            printf("IR: %u, Goals: %u\r\n",
                ir_value, goal_counter);
        }
    }
}