#ifndef __PANIC_HPP
#define __PANIC_HPP

#include <hardware/idt.hpp>

#define KPANIC(message) panic(message, __FILE__, __LINE__, nullptr)
#define PANIC(message, frame) panic(message, __FILE__, __LINE__, frame)

void panic(char* message, char* file, int line, TrapFrame* frame);

#endif