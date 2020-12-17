#ifndef __IDT_HPP
#define __IDT_HPP

#include <types.hpp>
#include <hardware/port.hpp>
#include <lib/singleton.hpp>

#define HARDWARE_INTERRUPT_OFFSET 0x20

struct TrapFrame {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;

    u32 esi;
    u32 edi;
    u32 ebp;

    u32 error;

    u32 eip;
    u32 cs;
    u32 flags;
    u32 esp;
    u32 ss;
} __attribute__((packed));

class InterruptHandler;

class InterruptManager : public Singleton<InterruptManager> {
public:
    InterruptHandler* handlers[256];

    InterruptManager();

    void activate();
    void deactivate();

private:
    struct IDTEntry {
        u16 lowBits;
        u16 codeSegment;
        u8 zero;
        u8 type;
        u16 highBits;
    } __attribute__((packed));

    struct IDTPointer {
        u16 size;
        u32 base;
    } __attribute__((packed));

    IDTEntry idt[256];

    Port8BitSlow masterCommandPort;
    Port8BitSlow masterDataPort;
    Port8BitSlow slaveCommandPort;
    Port8BitSlow slaveDataPort;

    void setIDTEntry(u8 interrupt, void (*handler)(), u16 codeSegment, u8 flags = 0);

    u32 handleException(u8 exception, u32 esp);

    static u32 handleInterrupt(u8 interrupt, u32 esp);
    u32 doHandleInterrupt(u8 interrupt, u32 esp);

    static void handleException0x00() { }
    static void handleException0x01() { }
    static void handleException0x02() { }
    static void handleException0x03() { }
    static void handleException0x04() { }
    static void handleException0x05() { }
    static void handleException0x06() { }
    static void handleException0x07() { }
    static void handleException0x08() { }
    static void handleException0x09() { }
    static void handleException0x0A() { }
    static void handleException0x0B() { }
    static void handleException0x0C() { }
    static void handleException0x0D() { }
    static void handleException0x0E() { }
    static void handleException0x0F() { }
    static void handleException0x10() { }
    static void handleException0x11() { }
    static void handleException0x12() { }
    static void handleException0x13() { }

    static void handleInterrupt0x00() { }
    static void handleInterrupt0x01() { }
    static void handleInterrupt0x02() { }
    static void handleInterrupt0x03() { }
    static void handleInterrupt0x04() { }
    static void handleInterrupt0x05() { }
    static void handleInterrupt0x06() { }
    static void handleInterrupt0x07() { }
    static void handleInterrupt0x08() { }
    static void handleInterrupt0x09() { }
    static void handleInterrupt0x0A() { }
    static void handleInterrupt0x0B() { }
    static void handleInterrupt0x0C() { }
    static void handleInterrupt0x0D() { }
    static void handleInterrupt0x0E() { }
    static void handleInterrupt0x0F() { }
    static void handleInterrupt0x80() { }

    static void ignoreInterrupt() { }
};

class InterruptHandler {
public:
    InterruptHandler(u8 interrupt);
    ~InterruptHandler();

    virtual u32 handle(u32 esp);

protected:
    u8 interrupt;
};

#endif