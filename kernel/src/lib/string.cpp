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

int strcmp(const char* a, const char* b) {
    while (*a) {
        if (*a != *b) {
            break;
        }

        a++;
        b++;
    }

    return *(const unsigned char*) a - *(const unsigned char*) b;
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

    u32 hexSize = 0;
    int temp = n;
    do {
        hexSize++;
        temp /= 16;
    } while (temp);

    char* rc = new char[hexSize + 1];
    memset(rc, 0, hexSize + 1);

    for (u32 i = 0, j = (hexSize - 1) * 4; i < hexSize; ++i, j -= 4) {
        rc[i] = digits[(n >> j) & 0x0f];
    }

    return rc;
}

int indexOf(const char* str, char c, u32 skip) {
    u32 hits = 0;
    int i = 0;

    while (str[i]) {
        if (str[i] == c && hits++ == skip) {
            return i;
        }

        i++;
    }

    return -1;
}

Vector<char*> split(const char* str, char delim) {
    Vector<char*> result;

    int amountOfDelims = 0;
    while (indexOf(str, delim, amountOfDelims) != -1) {
        amountOfDelims++;
    }

    if (amountOfDelims == 0) {
        return result;
    }

    int* delimOffsets = new int[amountOfDelims];
    for (int i = 0; i < amountOfDelims; i++) {
        delimOffsets[i] = indexOf(str, delim, i);
    }

    for (int i = 0; i < amountOfDelims; i++) {
        int len = i >= 1 ? (delimOffsets[i] - delimOffsets[i - 1] - 1) : delimOffsets[i];

        char* partStr = new char[len + 1];
        memcpy(partStr, (void*) (str + (i >= 1 ? delimOffsets[i - 1] + 1 : 0)), len);
        partStr[len] = '\0';

        result.pushBack(partStr);
    }

    int stringRemainder = strlen(str) - delimOffsets[amountOfDelims - 1];
    char* lastStr = new char[stringRemainder];
    memcpy(lastStr, (void*) (str + delimOffsets[amountOfDelims - 1] + 1), stringRemainder);
    lastStr[stringRemainder] = '\0';

    result.pushBack(lastStr);

    return result;
}