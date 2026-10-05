
#include "can.h"
#include "mcpdriver.h"
#include <util/delay.h>
#include <stdio.h>

static bool can_set_mode(enum mcp_modes mode);

bool can_init(enum mcp_modes mode)
{
    // Initialize the MCP2515 CAN controller
    uint8_t config_mode = MCP_MODE_CONFIG;
    // Set the configuration registers for the desired CAN bus speed and timing
    uint8_t cnf1 = 0x00;
    uint8_t cnf2 = 0xA5;
    uint8_t cnf3 = 0x03; 

    uint8_t rxb0cntrl = 0x64; // Accept all valid standard and extended messages, with rollover to RXB1
    uint8_t rxb1cntrl = 0x60; // Accept all valid standard and extended messages

    uint8_t interrupt_flags = 0x00; // Clear interrupt flags

    uint8_t interrupts = 0x03; // Enable interrupts for both recieve buffers RX0 and RX1
    uint8_t requested_mode = mode;

    mcp_reset();
    _delay_ms(1);
    mcp_write(MCP_CANCTRL, &config_mode, 1); // Set the MCP2515 to configuration mode

    mcp_write(MCP_CNF1, &cnf1, 1);
    mcp_write(MCP_CNF2, &cnf2, 1);
    mcp_write(MCP_CNF3, &cnf3, 1);

    mcp_write(MCP_RXB0CTRL, &rxb0cntrl, 1);
    mcp_write(MCP_RXB1CTRL, &rxb1cntrl, 1);

    mcp_write(MCP_CANINTF, &interrupt_flags, 1);

    mcp_write(MCP_CANINTE, &interrupts, 1);

    mcp_write(MCP_CANCTRL, &requested_mode, 1); // Set the MCP2515 mode (e.g., normal, sleep, loopback)

    if (!can_set_mode(mode)) {
        // Handle MCP2515 initialization failure
        printf("MCP2515 initialization failed. Please check the connections and try again.\n");
        return false;
    }
    
    return true;
}

static bool can_set_mode(enum mcp_modes mode)
{
    uint8_t status;
    uint8_t requested_mode = mode;

    mcp_write(MCP_CANCTRL, &requested_mode, 1);
    _delay_ms(1);

    mcp_read(MCP_CANSTAT, &status, 1);

    return (status & 0xE0) == mode;
}

bool can_send_message(uint32_t id, const uint8_t* data, uint8_t length)
{
    uint8_t tx_control;
    uint8_t frame[13];

    // This implementation supports standard 11-bit CAN identifiers only.
    if (id > 0x7FF || data == NULL || length > 8) {
        return false;
    }

    // TXREQ is bit 3 of TXB0CTRL.
    mcp_read(MCP_TXB0CTRL, &tx_control, 1);
    if ((tx_control & 0x08) != 0) {
        return false; // TXB0 is still transmitting
    }

    frame[0] = (uint8_t)(id >> 3);          // SIDH: ID bits 10..3
    frame[1] = (uint8_t)((id & 0x07) << 5); // SIDL: ID bits 2..0
    frame[2] = 0x00;                        // EID8
    frame[3] = 0x00;                        // EID0
    frame[4] = length;                      // DLC

    for (uint8_t i = 0; i < length; i++) {
        frame[5 + i] = data[i];
    }

    mcp_write(MCP_TXB0SIDH, frame, (uint8_t)(5 + length));

    // Request transmission from TXB0.
    mcp_request_to_send(MCP_RTS_TXB0);

    return true;
}

bool can_receive_message(uint32_t* id, uint8_t* data, uint8_t* length)
{
    uint8_t interrupt_flags;
    uint8_t frame[13];
    uint8_t buffer_address;
    uint8_t interrupt_mask;
    uint8_t frame_length;

    if (id == NULL || data == NULL || length == NULL) {
        return false;
    }

    mcp_read(MCP_CANINTF, &interrupt_flags, 1);

    if ((interrupt_flags & 0x01) != 0) {
        buffer_address = MCP_RXB0SIDH;
        interrupt_mask = 0x01;
    } else if ((interrupt_flags & 0x02) != 0) {
        buffer_address = MCP_RXB1SIDH;
        interrupt_mask = 0x02;
    } else {
        return false;
    }

    // Read SIDH, SIDL, EID8, EID0, DLC and all eight data bytes.
    mcp_read(buffer_address, frame, sizeof(frame));

    // This API currently supports standard 11-bit frames only.
    if ((frame[1] & 0x08) != 0) {
        mcp_bit_modify(MCP_CANINTF, interrupt_mask, 0x00);
        return false;
    }

    frame_length = frame[4] & 0x0F;
    if (frame_length > 8) {
        mcp_bit_modify(MCP_CANINTF, interrupt_mask, 0x00);
        return false;
    }

    *id = ((uint32_t)frame[0] << 3) | ((uint32_t)frame[1] >> 5);
    *length = frame_length;

    for (uint8_t i = 0; i < frame_length; i++) {
        data[i] = frame[5 + i];
    }

    // Clear only the flag for the buffer that was consumed.
    mcp_bit_modify(MCP_CANINTF, interrupt_mask, 0x00);

    return true;
}