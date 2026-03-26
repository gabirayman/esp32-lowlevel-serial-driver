#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>

void led_init(void);
void led_on(void);
void led_off(void);
void led_blinking(uint8_t freq);
void led_blinking_stop(void);

#endif