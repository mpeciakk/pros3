#include <lib/string.hpp>

u32 strlen(const char* str) {
    u32 len = 0;

    while (str[len]) {
        len++;
    }

    return len;
}

void* memset(void* dest, char val, u32 count) {
    unsigned char* temp = (unsigned char*) dest;
    for (; count != 0; count--, temp[count] = val) {
    }
    return dest;
}