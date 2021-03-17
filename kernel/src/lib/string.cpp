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

char* intToString(int n) {
    static char ret[32];
    int numChars = 0;

    int temp = n;
    do {
        numChars++;
        temp /= 10;
    } while (temp);

    ret[numChars] = 0;

    int i = numChars - 1;
    do {
        ret[i--] = n % 10 + '0';
        n /= 10;
    } while (n);
    return ret;
}

char* intToString(u32 n) {
    static char ret[32];
    int numChars = 0;

    int temp = n;
    do {
        numChars++;
        temp /= 10;
    } while (temp);

    ret[numChars] = 0;

    int i = numChars - 1;
    do {
        ret[i--] = n % 10 + '0';
        n /= 10;
    } while (n);
    return ret;
}

char* intToHex(u32 n) {
    static const char* digits = "0123456789ABCDEF";
    u32 hexSize = sizeof(u32) << 1;
    char* rc = new char[hexSize + 1];
    memset(rc, 0, hexSize + 1);

    for (u32 i = 0, j = (hexSize - 1) * 4; i < hexSize; ++i, j -= 4) {
        rc[i] = digits[(n >> j) & 0x0f];
    }

    return rc;
}