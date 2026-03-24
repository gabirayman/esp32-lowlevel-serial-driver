#ifndef UART_DRIVER_H
#define UART_DRIVER_H
#include <stdint.h>
#include <stdbool.h>

void drv_uart_tx_init(int baud_rate);
void drv_uart_putc(char c);
void drv_uart_puts(const char *str);
void drv_uart_flush();

void drv_uart_rx_init();
bool drv_uart_read_byte(uint8_t *data);


#endif // UART_DRIVER_H