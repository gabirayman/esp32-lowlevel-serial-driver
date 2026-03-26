#ifndef UART_STR_H
#define UART_STR_H
#include <stdbool.h>

char drv_char_to_upper(char c);
bool drv_str_equals(const char* str1, const char* str2);

#endif // UART_STR_H