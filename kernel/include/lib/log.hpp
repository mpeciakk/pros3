#ifndef __LOG_HPP
#define __LOG_HPP

void printf(const char* __restrict format, ...);
void log(int level, const char* __restrict format, ...);
int printk(const char* __restrict format, ...);
void hexdump(void* ptr, int buflen);

#endif