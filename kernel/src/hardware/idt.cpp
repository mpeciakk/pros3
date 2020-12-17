#include <hardware/idt.hpp>
#include <panic.hpp>

char* exceptions[] = {"Division by zero",
                      "Debug",
                      "Non-maskable interrupt",
                      "Breakpoint",
                      "Detected overflow",
                      "Out-of-bounds",
                      "Invalid opcode",
                      "No coprocessor",
                      "Double fault",
                      "Coprocessor segment overrun",
                      "Bad TSS",
                      "Segment not present",
                      "Stack fault",
                      "General protection fault",
                      "Page fault",
                      "Unknown interrupt",
                      "Coprocessor fault",
                      "Alignment check",
                      "Machine check"};

InterruptManager::InterruptManager() : masterCommandPort(0x20), masterDataPort(0x21), slaveCommandPort(0xA0), slaveDataPort(0xA1) {
    this->instance = this;

    u16 codeSegment = 0x08;

    for (u8 i = 0; i < 255; i++) {
        setIDTEntry(i, &ignoreInterrupt, codeSegment);
    }

    setIDTEntry(0x00, &handleException0x00, codeSegment);
    setIDTEntry(0x01, &handleException0x01, codeSegment);
    setIDTEntry(0x02, &handleException0x02, codeSegment);
    setIDTEntry(0x03, &handleException0x03, codeSegment);
    setIDTEntry(0x04, &handleException0x04, codeSegment);
    setIDTEntry(0x05, &handleException0x05, codeSegment);
    setIDTEntry(0x06, &handleException0x06, codeSegment);
    setIDTEntry(0x07, &handleException0x07, codeSegment);
    setIDTEntry(0x08, &handleException0x08, codeSegment);
    setIDTEntry(0x09, &handleException0x09, codeSegment);
    setIDTEntry(0x0A, &handleException0x0A, codeSegment);
    setIDTEntry(0x0B, &handleException0x0B, codeSegment);
    setIDTEntry(0x0C, &handleException0x0C, codeSegment);
    setIDTEntry(0x0D, &handleException0x0D, codeSegment);
    setIDTEntry(0x0E, &handleException0x0E, codeSegment);
    setIDTEntry(0x0F, &handleException0x0F, codeSegment);
    setIDTEntry(0x10, &handleException0x10, codeSegment);
    setIDTEntry(0x11, &handleException0x11, codeSegment);
    setIDTEntry(0x12, &handleException0x12, codeSegment);
    setIDTEntry(0x13, &handleException0x13, codeSegment);

    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x00, &handleInterrupt0x00, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x01, &handleInterrupt0x01, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x02, &handleInterrupt0x02, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x03, &handleInterrupt0x03, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x04, &handleInterrupt0x04, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x05, &handleInterrupt0x05, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x06, &handleInterrupt0x06, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x07, &handleInterrupt0x07, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x08, &handleInterrupt0x08, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x09, &handleInterrupt0x09, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0A, &handleInterrupt0x0A, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0B, &handleInterrupt0x0B, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0C, &handleInterrupt0x0C, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0D, &handleInterrupt0x0D, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0E, &handleInterrupt0x0E, codeSegment);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0F, &handleInterrupt0x0F, codeSegment);
    setIDTEntry(0x80, &handleInterrupt0x80, codeSegment, 0x60);

    masterCommandPort.write(0x11);
    slaveCommandPort.write(0x11);

    masterDataPort.write(HARDWARE_INTERRUPT_OFFSET);
    slaveDataPort.write(HARDWARE_INTERRUPT_OFFSET + 8);

    masterDataPort.write(0x04);
    slaveDataPort.write(0x02);

    masterDataPort.write(0x01);
    slaveDataPort.write(0x01);

    masterDataPort.write(0x00);
    slaveDataPort.write(0x00);

    IDTPointer pointer;
    pointer.size = 256 * sizeof(IDTEntry) - 1;
    pointer.base = (u32) idt;
    asm volatile("lidt %0" : : "m"(pointer));
}

void InterruptManager::activate() {
    asm("sti");
}

void InterruptManager::deactivate() {
    asm("cli");
}

void InterruptManager::setIDTEntry(u8 interrupt, void (*handler)(), u16 codeSegment, u8 flags) {
    u32 address = (u32) handler;

    idt[interrupt].lowBits = address & 0xFFFF;
    idt[interrupt].highBits = (address >> 16) & 0xFFFF;
    idt[interrupt].codeSegment = codeSegment;
    idt[interrupt].type = 0x80 | ((0 & 3) << 5) | 0xE | flags;
    idt[interrupt].zero = 0;
}

u32 InterruptManager::handleException(u8 exception, u32 esp) {
    TrapFrame* frame = (TrapFrame*) esp;

    PANIC(exceptions[exception], frame);

    deactivate();

    return esp;
}

u32 InterruptManager::handleInterrupt(u8 interrupt, u32 esp) {
    return InterruptManager::instance->doHandleInterrupt(interrupt, esp);
}

u32 InterruptManager::doHandleInterrupt(u8 interrupt, u32 esp) {
    if (interrupt <= 0x13) {
        handleException(interrupt, esp);
    }

    if (handlers[interrupt] != nullptr) {
        esp = handlers[interrupt]->handle(esp);
    }

    if (0x20 <= interrupt && interrupt < 0x30) {
        masterCommandPort.write(0x20);

        if (0x28 <= interrupt) {
            slaveCommandPort.write(0x20);
        }
    }

    return esp;
}

InterruptHandler::InterruptHandler(u8 interrupt) {
    InterruptManager::instance->handlers[interrupt] = this;
}

InterruptHandler::~InterruptHandler() {
    if (InterruptManager::instance->handlers[interrupt] == this) {
        InterruptManager::instance->handlers[interrupt] = nullptr;
    }
}

u32 InterruptHandler::handle(u32 esp) {
    return esp;
}
