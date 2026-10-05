#ifndef MCPDRIVER_H
#define MCPDRIVER_H

#include <stdint.h>

// Driver for MCP2515 CAN controller

enum mcp_registers {
    MCP_BFPCTRL = 0x0C,
    MCP_TXRTSCTRL = 0x0D,
    MCP_CANSTAT = 0x0E,
    MCP_CANCTRL = 0x0F,
    MCP_TEC = 0x1C,
    MCP_REC = 0x1D,
    MCP_CNF3 = 0x28,
    MCP_CNF2 = 0x29,
    MCP_CNF1 = 0x2A,
    MCP_CANINTE = 0x2B,
    MCP_CANINTF = 0x2C,
    MCP_EFLG = 0x2D,
    MCP_TXB0CTRL = 0x30,
    MCP_TXB1CTRL = 0x40,
    MCP_TXB2CTRL = 0x50,
    MCP_RXB0CTRL = 0x60,
    MCP_RXB1CTRL = 0x70,
    MCP_TXB0SIDH = 0x31,
    MCP_TXB0DLC  = 0x35,
    MCP_RXB0SIDH = 0x61,
    MCP_RXB1SIDH = 0x71,
};

enum mcp_command {
    MCP_RESET = 0xC0,
    MCP_READ = 0x03,
    MCP_WRITE = 0x02,
    MCP_BITMOD = 0x05,
    MCP_READ_STATUS = 0xA0,
    MCP_RX_STATUS = 0xB0,
    MCP_RTS = 0x80,
    MCP_RTS_TXB0 = 0x81,
    MCP_RTS_TXB1 = 0x82,
    MCP_RTS_TXB2 = 0x84
};

enum mcp_modes {
    MCP_MODE_NORMAL = 0x00,
    MCP_MODE_SLEEP = 0x20,
    MCP_MODE_LOOPBACK = 0x40,
    MCP_MODE_LISTENONLY = 0x60,
    MCP_MODE_CONFIG = 0x80
};

void mcp_read(uint8_t address, uint8_t* data, uint8_t length);
void mcp_write(uint8_t address, const uint8_t* data, uint8_t length);
void mcp_request_to_send(uint8_t tx_buffer);
void mcp_read_status(uint8_t* status);
void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t data);
void mcp_reset(void);

#endif // MCPDRIVER_H