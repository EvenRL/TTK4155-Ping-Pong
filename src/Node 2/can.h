
// TTK4155 Handout

#pragma once

#include <stdint.h>

// Struct with bit timing information

typedef struct CanInit CanInit;
__attribute__((packed)) struct CanInit {
    union {
        struct {
            uint32_t phase2:4;  // Phase 2 segment
            uint32_t propag:4;  // Propagation time segment
            uint32_t phase1:4;  // Phase 1 segment
            uint32_t sjw:4;     // Synchronization jump width
            uint32_t brp:8;     // Baud rate prescaler
            uint32_t smp:8;     // Sampling mode
        };
        uint32_t reg;
    };
};


// Initialize CAN bus, with bit timings and optional interrupt
// If `rxInterrupt` is not 0, an interrupt will be triggered when a message is received.

void can_init(CanInit init, uint8_t rxInterrupt);


// Strict-aliasing-safe reinterpret-cast
#define union_cast(type, x) \
    (((union { \
        typeof(x) a; \
        type b; \
    })x).b)


// Dummy type for use with `union_cast`, see below
typedef struct Byte8 Byte8;
struct Byte8 {
    uint8_t bytes[8];
};

typedef struct CanMsg CanMsg;
struct CanMsg {
    uint8_t id;
    uint8_t length;
    union {
        uint8_t     byte[8];
        uint32_t    dword[2];
        Byte8       byte8;
    };    
};

// Send a CAN message on the bus. 

void can_tx(CanMsg m);

// Receive a CAN message.

uint8_t can_rx(CanMsg* m);

// Print a CAN message (using `printf`)
void can_printmsg(CanMsg m);





