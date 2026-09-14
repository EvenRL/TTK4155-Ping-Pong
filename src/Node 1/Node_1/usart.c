
#define F_CPU 4915200UL

#include "usart.h"

#define USART_RX_BUFFER_SIZE 32

// =========================================================
//                     RX BUFFER
// =========================================================

static volatile uint8_t rx_buffer[USART_RX_BUFFER_SIZE];

static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;


// =========================================================
//                      USART INIT
// =========================================================

void usart_init(uint32_t baud)
{
	// F_CPU = 4.9152 MHz
	// baud  = 9600
	// UBRR = 31
	uint16_t ubrr = (uint16_t)(F_CPU / (16UL * baud) - 1);

	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	// Enable receiver and transmitter
	// RXCIE0 enables interrupt when a new character is received.
	UCSR0B =
	(1 << RXEN0)  |
	(1 << TXEN0)  |
	(1 << RXCIE0);

	// USART config:
	// Async mode, 8 data, 0 parity, 1 stop bit
	UCSR0C =
	(1 << URSEL0) |
	(1 << UCSZ01) |
	(1 << UCSZ00);

	// Enable global interrupts.
	//sei();
}


// =========================================================
//                      TRANSMIT
// =========================================================

void usart_tx(uint8_t data)
{
	// Wait until the USART data register is empty.
	// If the USART is still busy with the previous
	// character, the program waits here
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}

	// Put the new character in the USART data register.
	UDR0 = data;
}


//void usart_puts(const char *str)
//{
	//while (*str)
	//{
		//usart_tx(*str);
		//str++;
	//}
//}


// =========================================================
//                       RECEIVE
// =========================================================

bool usart_rx(uint8_t *data)
{
	// If head == tail, the buffer is empty.
	if (rx_head == rx_tail)
	{
		return false;
	}

	// Read next character from buffer
	*data = rx_buffer[rx_tail];


	// Move tail to next position.
	rx_tail++;

	if (rx_tail >= USART_RX_BUFFER_SIZE)
	{
		rx_tail = 0;
	}

	return true;
}

bool usart_data_available(void)
{
	return rx_head != rx_tail;
}


// =========================================================
//                    RX INTERRUPT
// =========================================================

ISR(USART0_RXC_vect)
{
	// Reading UDR0 gives us the received character.
	uint8_t data = UDR0;

	// Calculate where the next character would be stored.
	uint8_t next_head = rx_head + 1;

	if (next_head >= USART_RX_BUFFER_SIZE)
	{
		next_head = 0;
	}

	// If next_head == rx_tail, the buffer is full.
	// In that case we simply discard the new character.
	if (next_head != rx_tail)
	{
		rx_buffer[rx_head] = data;
		rx_head = next_head;
	}
}


// =========================================================
//                    PRINTF CONNECTION
// =========================================================

static int usart_putchar(char c, FILE *stream)
{
	//
	if (c == '\n')
	{
		usart_tx('\r');
	}
	
	usart_tx((uint8_t)c);
	
	return 0;
}

static FILE usart_stdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

void usart_init_printf(void)
{
	stdout = &usart_stdout;
}
