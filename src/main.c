#include <string.h>
#include "uart_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_CMD_LEN 64

void app_main(void) {
    // 1. Initialize the Hardware and the Interrupts
    drv_uart_tx_init(115200);

    drv_uart_puts("\r\n--- ESP32 Bare-Metal CLI Ready ---\r\n");
    drv_uart_puts("Commands: 'HELLO', 'RESET'\r\n");
    drv_uart_rx_init();

    char cmd_buffer[MAX_CMD_LEN];
    int cmd_index = 0;

    while (1) {
        uint8_t byte;
        
        // read our byte from the ring buffer filled by the ISR
        if (drv_uart_read_byte(&byte)) {
            
            // Echo the character back so the user sees what they type
            drv_uart_putc((char)byte);

            // Check for End of Line (Enter key)
            if (byte == '\r' || byte == '\n') {
                cmd_buffer[cmd_index] = '\0'; // Null-terminate the string
                drv_uart_puts("\r\n");        // Move to next line

                // Command Logic
                if ( (strcmp(cmd_buffer, "HELLO") == 0) || (strcmp(cmd_buffer, "hello") == 0) ) {
                    drv_uart_puts("Hi there, Gabi! Driver is working.\r\n");
                } 
                else if (strcmp(cmd_buffer, "RESET") == 0) {
                    drv_uart_puts("Rebooting system...\r\n");
                    drv_uart_flush();
                    esp_restart();
                } 
                else if (cmd_index > 0) {
                    drv_uart_puts("Unknown Command: ");
                    drv_uart_puts(cmd_buffer);
                    drv_uart_puts("\r\n");
                }

                // Reset index for next command
                cmd_index = 0;
            } 
            else {
                // Store character if there's room
                if (cmd_index < MAX_CMD_LEN - 1) {
                    cmd_buffer[cmd_index++] = (char)byte;
                }
            }
        }

        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}