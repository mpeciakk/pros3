#ifndef __LOG_HPP
#define __LOG_HPP

int log(int level, const char* __restrict format, ...);
int printk(const char* __restrict format, ...);

#endif