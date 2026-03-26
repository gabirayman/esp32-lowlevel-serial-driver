#include "uart_driver.h"
#include "uart_str.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cli_commands.h"
#include "uart_cli.h"
#include "led_driver.h"

#define MAX_CMD_LEN 64

void process_cli_input(char* input_buffer) {
    // Convert input to uppercase 
    for (int i = 0; input_buffer[i] != '\0'; i++) {
        input_buffer[i] = drv_char_to_upper(input_buffer[i]);
    }

    // Lookup
    for (int i = 0; cli_commands[i].name != NULL; i++) {
        if (drv_str_equals(input_buffer, cli_commands[i].name)) {
            cli_commands[i].handler(); // Execute
            return;
        }
    }
    drv_uart_puts("Unknown Command: ");
    drv_uart_puts(input_buffer);
    drv_uart_puts("\r\n");
    drv_uart_puts("> ");
}

void cli_run(void) {
    // Initialize the Hardware and the Interrupts
    drv_uart_tx_init(115200);

    drv_uart_puts("\r\n--- ESP32 Bare-Metal CLI Ready ---\r\n");
    drv_uart_puts("> ");
    drv_uart_rx_init();
    // initialize the LED for the led commands
    led_init();

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
                drv_uart_puts("\r\n> ");        // Move to next line

                process_cli_input(cmd_buffer);

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