#ifndef __LOG_HPP
#define __LOG_HPP

#define INT_MAX 2147483647

void printf(const char* __restrict format, ...);
void log(int level, const char* __restrict format, ...);
int printk(const char* __restrict format, ...);
void hexdump(void* ptr, int len);

#endif