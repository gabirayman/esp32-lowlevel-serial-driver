#include "cli_commands.h"
#include "uart_driver.h"
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_driver.h"

//functions
void cmd_reset(void) { esp_restart(); }
void cmd_status(void) { drv_uart_puts("System OK\r\n"); }
void cmd_led_on(void) { led_on(); }
void cmd_led_off(void) { led_off(); }
void cmd_led_blink_1Hz(void) { led_blinking(1); }
void cmd_led_blink_2Hz(void) { led_blinking(2); }
void cmd_led_blink_3Hz(void) { led_blinking(3); }
void cmd_led_blink_4Hz(void) { led_blinking(4); }
void cmd_led_blink_5Hz(void) { led_blinking(5); }



// Command list
cli_cmd_t cli_commands[] = {
    {"RESET", cmd_reset, "Reset the system"},
    {"STATUS", cmd_status, "Get system status"},
    {"LED ON", cmd_led_on, "Turn LED on"},
    {"LED OFF", cmd_led_off, "Turn LED off"},
    {"LED BLINK", cmd_led_blink_1Hz, "Blink LED at 1Hz"},
    {"LED BLINK 1", cmd_led_blink_1Hz, "Blink LED at 1Hz"},
    {"LED BLINK 2", cmd_led_blink_2Hz, "Blink LED at 2Hz"},
    {"LED BLINK 3", cmd_led_blink_3Hz, "Blink LED at 3Hz"},
    {"LED BLINK 4", cmd_led_blink_4Hz, "Blink LED at 4Hz"},
    {"LED BLINK 5", cmd_led_blink_5Hz, "Blink LED at 5Hz"},
    {NULL, NULL, NULL}
};