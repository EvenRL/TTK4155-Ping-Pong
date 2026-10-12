#ifndef CAN_H
#define CAN_H

#include "mcpdriver.h"
#include <stdint.h>
#include <stdbool.h>

bool can_init(enum mcp_modes mode);
bool can_send_message(uint32_t id, const uint8_t* data, uint8_t length);
bool can_receive_message(uint32_t* id, uint8_t* data, uint8_t* length);
bool can_take_rx_pending(void);

#endif // CAN_H