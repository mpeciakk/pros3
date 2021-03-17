#ifndef __STRING_HPP
#define __STRING_HPP

#include <types.hpp>

#define INT32_MAX_DIGITS 20
#define INT32_MAX_HEX_DIGITS 8

u32 strlen(const char* str);
void* memset(void* dest, char val, u32 count);
void memcpy(void* dest, void* src, u32 size);
char* intToString(int n);
char* intToString(u32 n);
char* intToHex(u32 n);

#endif
