#include "led_driver.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_ulp.h" 

// -- GPIO Register Definitions --
#define GPIO_ENABLE_REG 0x3FF44020
#define GPIO_SET_REG    0x3FF44008
#define GPIO_CLEAR_REG  0x3FF4400C
#define GPIO_PIN_2      (1 << 2)

// -- Timer Register Definitions --
#define T0_CONFIG_REG   0x3FF5F000
#define T0_EN           (1 << 31)
#define T0_INCREASE     (1 << 30)
#define T0_DIVIDER_INT  80 
#define T0_DIVIDER_OFF  13
#define T0_AUTORELOAD   (1 << 29)
#define T0_ALARM_EN     (1 << 10)
#define CONFIG_STATIC   T0_INCREASE | (T0_DIVIDER_INT << T0_DIVIDER_OFF) | T0_AUTORELOAD

#define T0_ALARMLO_REG  0x3FF5F010
#define T0_ALARMHI_REG  0x3FF5F014
#define T0_LOADLO_REG   0x3FF5F018
#define T0_LOADHI_REG   0x3FF5F01C
#define T0_LOAD_ACT_REG 0x3FF5F020
#define ENA_REG         0x3FF5F098
#define CLR_REG         0x3FF5F0A4

// Internal state
static volatile int led_is_on = 0;
// used as an id for the interrupt handler to disable it when needed
static intr_handle_t timer_intr_handle = NULL;

// ISR Handler
void IRAM_ATTR timer_isr_handler(void* arg) {
    *(volatile uint32_t *)CLR_REG = 1;         // Clear interrupt
    *(volatile uint32_t *)T0_CONFIG_REG |= T0_ALARM_EN; // Re-enable alarm

    if (led_is_on) {
        *(volatile uint32_t *)GPIO_CLEAR_REG = GPIO_PIN_2;
        led_is_on = 0;
    } else {
        *(volatile uint32_t *)GPIO_SET_REG = GPIO_PIN_2;
        led_is_on = 1;
    }
}

void led_init(void) {
    // Enable GPIO 2 as output
    *(volatile uint32_t *)GPIO_ENABLE_REG |= GPIO_PIN_2;
    
    // Register the interrupt handler once
    esp_intr_alloc(ETS_TG0_T0_LEVEL_INTR_SOURCE, 0, timer_isr_handler, NULL, &timer_intr_handle);
    esp_intr_disable(timer_intr_handle);
}

void led_on(void) {
    led_blinking_stop(); // Stop blinking if active
    *(volatile uint32_t *)GPIO_SET_REG = GPIO_PIN_2;
    led_is_on = 1;
}

void led_off(void) {
    led_blinking_stop(); // Stop blinking if active
    *(volatile uint32_t *)GPIO_CLEAR_REG = GPIO_PIN_2;
    led_is_on = 0;
}

void led_blinking(uint8_t freq) {
    if (freq < 1) freq = 1;
    if (freq > 5) freq = 5;

    // initialize the timer for blinking
    // Force Stop the timer and clear config
    *(volatile uint32_t *)T0_CONFIG_REG &= ~T0_EN;

    *(volatile uint32_t *)T0_CONFIG_REG |= CONFIG_STATIC;

    // Set alarm value (500,000 / freq)
    uint32_t alarm_val = 500000 / freq;
    *(volatile uint32_t *)T0_ALARMLO_REG = alarm_val;
    *(volatile uint32_t *)T0_ALARMHI_REG = 0;

    // Enable the interrupt handle and the ENA_REG
    *(volatile uint32_t *)ENA_REG = 1;

    *(volatile uint32_t *)T0_CONFIG_REG |= T0_ALARM_EN;

    *(volatile uint32_t *)T0_CONFIG_REG |= T0_EN;

    // Load 0 into the counter
    *(volatile uint32_t *)T0_LOADLO_REG = 0;
    *(volatile uint32_t *)T0_LOADHI_REG = 0;
    *(volatile uint32_t *)T0_LOAD_ACT_REG = 1;

    *(volatile uint32_t *)T0_CONFIG_REG |= T0_ALARM_EN;

    
    esp_intr_enable(timer_intr_handle);
}

void led_blinking_stop(void) {
    // Disable timer and interrupt
    *(volatile uint32_t *)T0_CONFIG_REG &= ~T0_EN;
    esp_intr_disable(timer_intr_handle);
}