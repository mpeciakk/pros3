#include <Hardware/Port.hpp>

Port::Port(u16 portNumber) {
    this->portNumber = portNumber;
}

Port8Bit::Port8Bit(u16 portNumber) : Port(portNumber) { }

void Port8Bit::write(u8 data) {
    asm volatile("outb %0, %1" ::"a"(data), "Nd"(portNumber));
}

u8 Port8Bit::read() {
    u8 result;

    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

Port8BitSlow::Port8BitSlow(u16 portNumber) : Port8Bit(portNumber) { }

void Port8BitSlow::write(u8 data) {
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" ::"a"(data), "Nd"(portNumber));
}

u8 Port8BitSlow::read() {
    u8 result;

    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

Port16Bit::Port16Bit(u16 portNumber) : Port(portNumber) { }

void Port16Bit::write(u16 data) {
    asm volatile("outw %0, %1" ::"a"(data), "Nd"(portNumber));
}

u16 Port16Bit::read() {
    u16 result;

    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

Port32Bit::Port32Bit(u16 portNumber) : Port(portNumber) { }

void Port32Bit::write(u32 data) {
    asm volatile("outl %0, %1" ::"a"(data), "Nd"(portNumber));
}

u32 Port32Bit::read() {
    u32 result;

    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}