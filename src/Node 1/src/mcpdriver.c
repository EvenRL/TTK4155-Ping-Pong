
#include "mcpdriver.h"
#include "SPI.h"

void mcp_read(uint8_t address, uint8_t* data, uint8_t length)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_READ);
    SPI_MasterTransmit(address);
    for (uint8_t i = 0; i < length; i++)
    {
        data[i] = SPI_MasterReceive();
    }
    mcp2515_deselect();
}

void mcp_write(uint8_t address, const uint8_t* data, uint8_t length)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_WRITE);
    SPI_MasterTransmit(address);
    for (uint8_t i = 0; i < length; i++)
    {
        SPI_MasterTransmit(data[i]);
    }
    mcp2515_deselect();
}

void mcp_request_to_send(uint8_t tx_buffer)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_RTS | (tx_buffer & 0x07));
    mcp2515_deselect();
}

void mcp_read_status(uint8_t* status)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_READ_STATUS);
    *status = SPI_MasterReceive();
    mcp2515_deselect();
}

void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_BITMOD);
    SPI_MasterTransmit(address);
    SPI_MasterTransmit(mask);
    SPI_MasterTransmit(data);
    mcp2515_deselect();
}

void mcp_reset(void)
{
    mcp2515_select();
    SPI_MasterTransmit(MCP_RESET);
    mcp2515_deselect();
}