#include "uart_str.h"
#include <stdbool.h>

// function to convert a character to uppercase
char drv_char_to_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        // convert to uppercase by flipping the 5th bit (-32)
        c ^= 0x20;
    }
    return c;
}

bool drv_str_equals(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return false;
        }
        str1++;
        str2++;
    }
    return *str1 == *str2;
}
