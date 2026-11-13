
// Initialize. Hooks stdio functions (like `printf`)
void uart_init(uint32_t cpufreq, uint32_t baudrate);

// Send a single character
// Prefer using `printf` instead
void uart_tx(uint8_t val);

// Read a single character
// Prefer using `uart_flush` and `sscanf` instead (see below)
uint8_t uart_rx(uint8_t* val);

// Flush the internal ring buffer into your own buffer : 

int uart_flush(char* buf, int len);

   