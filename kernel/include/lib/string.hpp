#ifndef __STRING_HPP
#define __STRING_HPP

#include <lib/vector.hpp>
#include <types.hpp>

template <class T> class Vector;

u32 strlen(const char* str);
void* memset(void* dest, char val, u32 count);
void memcpy(void* dest, void* src, u32 size);
int strcmp(const char* a, const char* b);
char* intToString(int n);
char* intToString(u32 n);
char* intToHex(u32 n);
int indexOf(const char* str, char c, u32 skip);
Vector<char*> split(const char* str, char delim);

#endif
