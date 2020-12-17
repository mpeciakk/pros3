#include <multiboot.hpp>
#include <terminal.hpp>
#include <types.hpp>
#include <hardware/gdt.hpp>
#include <hardware/idt.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, u32 multibootMagic) {
    Terminal terminal;
    terminal.clear();

    terminal.write("Initializing GDT\n");
    GDT gdt;

    int stack = 0;
    asm volatile("mov %%esp, %0" : "=r"(stack));
    setTSS(0x10, stack);

    terminal.write("Loading interrupt manager\n");
    InterruptManager interruptManager;

    terminal.write("Activating interrupts\n");
    interruptManager.activate();

    terminal.write("Kernel loaded!");

    while (true) {
    }
}
