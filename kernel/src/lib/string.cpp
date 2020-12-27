#include <climits>
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

void memcpy(void* dest, void* src, u32 size) {
    char* csrc = (char*) src;
    char* cdest = (char*) dest;

    for (u32 i = 0; i < size; i++) {
        cdest[i] = csrc[i];
    }
}

void intToString(int n, char* result) {
    if (!n) {
        result[0] = '0';
        return;
    }

    if (n == INT_MIN) {
        char intMinString[] = "-2147483648";
        memcpy(result, intMinString, strlen(intMinString));

        return;
    }

    int digits = 0;
    int stopAt = 0;

    if (n >> 31) {
        n = ~n + 1;
        for (int temp = n; temp != 0; temp /= 10, digits++) {
        }

        result[0] = '-';
        digits++;

        stopAt = 1;

    } else {
        for (int temp = n; temp != 0; temp /= 10, digits++) {
        }
    }

    for (int i = digits - 1, temp = n; i >= stopAt; i--) {
        result[i] = (temp % 10) + '0';
        temp /= 10;
    }
}

void intToHex(int n, char* result) {
    if (!n) {
        result[0] = '0';
        return;
    }

    for (int i = 0; n != 0; n = n / 0x10, i++) {
        u8 rem = n % 0x10;

        if (rem < 10) {
            result[i] = rem + 48;
        } else {
            result[i] = rem + 55;
        }
    }

    u32 resultLength = strlen(result);
    for (u32 i = 0, k = resultLength - 1; i < (resultLength / 2); i++, k--) {
        int temp = result[k];
        result[k] = result[i];
        result[i] = temp;
    }
}