# ESP32 Bare-Metal CLI & Peripheral Drivers

A low-level firmware project for the ESP32 that implements a custom Command Line Interface (CLI) by interacting directly with hardware registers. This project bypasses standard high-level APIs to demonstrate deep understanding of MCU architecture, interrupt handling, and driver development.

## Overview

This project serves as a lightweight "operating system" shell for the ESP32. It features a fully functional UART driver and a Timer/GPIO driver, both written from the ground up using **direct register manipulation**. The CLI allows real-time hardware control, such as manual LED state toggling and variable-frequency blinking using hardware timer interrupts.

## Key Technical Features

### 1. Bare-Metal UART Driver (`uart_driver.c`)
* **Direct Register Mapping**: Defined a custom `esp32_uart_t` struct mapped to the UART0 base address (`0x3FF40000`) to control hardware flow.
* **Interrupt-Driven RX**: Implemented an Interrupt Service Routine (ISR) using `IRAM_ATTR` to handle incoming data with minimal latency and high priority.
* **Circular Ring Buffer**: Developed a custom software ring buffer to bridge the gap between high-speed hardware interrupts and the CLI processing task, ensuring zero data loss during high-speed transmission.
* **Manual Baud Rate Calculation**: Calculated clock dividers and fractional parts based on the 80MHz APB clock to achieve precise 115200 baud communication.

### 2. Hardware Timer & LED Driver (`led_driver.c`)
* **Timer ISR**: Configured Timer Group 0, Timer 0 at the register level to handle precise timing intervals.
* **Frequency Modulation**: Implemented 5 different blinking frequencies (1Hz–5Hz) by dynamically updating the `T0_ALARMLO_REG` and reloading the hardware counter.
* **Resource Management**: Utilized `intr_handle_t` to manage interrupt allocation, allowing the system to safely enable/disable hardware interrupts during runtime.

### 3. Modular CLI Engine (`uart_cli.c`)
* **Command Lookup Table**: Uses a structured `cli_cmd_t` array for easy command expansion.
* **Case Insensitivity**: Includes a custom string utility to handle input formatting.
* **Non-Blocking Execution**: The CLI runs in a loop synchronized with FreeRTOS `vTaskDelay`, allowing the system to remain responsive while waiting for user input.

---

## Command List

Once connected via serial (115200 Baud), the following commands are available:

| Command | Action |
| :--- | :--- |
| `STATUS` | Returns the current system health and "System OK" confirmation. |
| `LED ON` | Manually forces the onboard LED to stay ON. |
| `LED OFF` | Manually forces the onboard LED to stay OFF. |
| `LED BLINK` | Starts the LED blinking at the default frequency (1Hz). |
| `LED BLINK [1-5]` | Starts blinking at the specified frequency (e.g., `LED BLINK 3` for 3Hz). |
| `RESET` | Triggers a software-controlled hardware restart of the ESP32. |

---

## Development Environment

* **IDE**: VS Code with **PlatformIO**.
* **Simulation**: **Wokwi**.
* **Framework**: ESP-IDF.

## Project Structure

* `main.c`: Entry point that launches the CLI runner.
* `uart_driver.c/h`: Low-level UART register definitions, baud rate configuration, and ISR.
* `led_driver.c/h`: Timer-based blinking logic and GPIO output control.
* `uart_cli.c/h`: The command processing engine and input buffer logic.
* `cli_commands.c/h`: Definitions of user-facing commands and their associated handler functions.
* `uart_str.c/h`: Custom lightweight str functions to handle user input.

---
