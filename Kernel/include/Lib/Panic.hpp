#ifndef __PANIC_HPP
#define __PANIC_HPP

#include <Hardware/IDT.hpp>

#define KPANIC(message) panic(message, nullptr)
#define PANIC(message, frame) panic(message, frame)

void panic(char* message, TrapFrame* frame);

#endif