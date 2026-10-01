#ifndef CAN_H
#define CAN_H

#include "mcpdriver.h"

void can_init(enum mcp_modes mode);
void can_send_message(uint32_t id, const uint8_t* data, uint8_t length);
void can_receive_message(uint32_t* id, uint8_t* data, uint8_t* length);

#endif // CAN_H