#include "uart_driver.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/interrupts.h"

// ring buffer size for received data in ISR from uart fifo
#define RING_BUF_SIZE 128

// struct for uart registers
typedef struct {
    volatile uint32_t fifo;      // 0x00: Data R/W 
    volatile uint32_t int_raw;   // 0x04: Raw interrupts 
    volatile uint32_t int_st;    // 0x08: Masked interrupts 
    volatile uint32_t int_ena;   // 0x0C: Enable interrupts 
    volatile uint32_t int_clr;   // 0x10: Clear interrupts 
    volatile uint32_t clk_div;   // 0x14: Baud rate 
    volatile uint32_t autobaud;  // 0x18: Autobaud 
    volatile uint32_t status;    // 0x1C: Current status 
    volatile uint32_t conf0;     // 0x20: Config 0 (Baud/Parity/Stop) 
    volatile uint32_t conf1;     // 0x24: Config 1 (FIFO Thresholds) 
} esp32_uart_t;

esp32_uart_t *UART0 = (esp32_uart_t *)0x3FF40000;

// helper function to check fifo status
int get_tx_fifo_cnt() {
    return (UART0->status >> 16) & 0xFF; // lower 8 bits indicate bytes in FIFO
}

// Driver: initialize UART
void drv_uart_tx_init(int baud_rate) {
    // Configure baud rate
    uint32_t divider = (80000000 / baud_rate);
    // fragment part of the baud rate divisor
    uint32_t fraction = ((80000000 % baud_rate) * 16) / baud_rate;

    UART0->clk_div = (divider << 0) | (fraction << 20);

    // Config: 8-bit (3), 1 stop bit (1), no parity (0)
    UART0->conf0 = (3 << 2) | (1 << 4);
}

// Driver: Write a single character
void drv_uart_putc(char c) {
    // Wait until there is space in the FIFO
    while (get_tx_fifo_cnt() >= 128) {
        // wait
    }

    // Write character to FIFO
    UART0->fifo = c; 
}

// Driver: Write a string
void drv_uart_puts(const char *str) {
    while (*str) {
        drv_uart_putc(*str++);
    }
}

// Driver: Flush the UART
void drv_uart_flush() {
    //wait until all bytes in FIFO are transmitted
    while (get_tx_fifo_cnt() != 0) {
        // Wait
    }
    
    // // Wait for FSM to return to TX_IDLE (0)
    while (((UART0->status >> 24) & 0x0F) != 0) {
        // Wait
    }
}

static bool rx_buffer_write(uint8_t data);

static void IRAM_ATTR uart_isr_handler(void* arg) {
    // We check this to ensure Bit 0 (RX FIFO Full) is actually what fired
    uint32_t intr_status = UART0->int_st;

    if (intr_status & (1 << 0)) {
        // Read all bytes in the RX FIFO (Bits 0-7 indicate bytes in FIFO)
        while ((UART0->status & 0xFF) > 0) {
            uint8_t received_byte = (uint8_t)UART0->fifo;
            rx_buffer_write(received_byte);
        }
    }

    UART0->int_clr = (1 << 0);
}

void drv_uart_rx_init() {
    // Clear any existing interrupt status
    UART0->int_clr = (1 << 0);

    // set RX FIFO threshold to 1
    UART0->conf1 |= (1 & 0x7F);

    UART0->int_ena |= (1 << 0);

    esp_intr_alloc(ETS_UART0_INTR_SOURCE, 0, uart_isr_handler, NULL, NULL);
}

static uint8_t rx_data_array[RING_BUF_SIZE];
static volatile size_t head = 0;
static volatile size_t tail = 0;

static bool is_rx_buffer_empty() {
    return head == tail;
}

static bool is_rx_buffer_full() {
    return ((head + 1) % RING_BUF_SIZE) == tail;
}

static bool rx_buffer_write(uint8_t data) {
    if (is_rx_buffer_full()) {
        return false; // Buffer is full
    }
    rx_data_array[head] = data;
    head = (head + 1) % RING_BUF_SIZE;
    return true;
}

// static bool rx_buffer_read(uint8_t *data) {
//     if (is_rx_buffer_empty()) {
//         return false; // Buffer is empty
//     }
//     *data = rx_data_array[tail];
//     tail = (tail + 1) % RING_BUF_SIZE;
//     return true;
// }

// static size_t rx_buffer_count() {
//     if (head >= tail) {
//         return head - tail;
//     } else {
//         return RING_BUF_SIZE - (tail - head);
//     }
// }

bool drv_uart_read_byte(uint8_t *data) {
    if (is_rx_buffer_empty()) {
        return false; // No data available
    }
    *data = rx_data_array[tail];
    tail = (tail + 1) % RING_BUF_SIZE;
    return true;
}

