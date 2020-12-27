#include <multiboot.hpp>
#include <terminal.hpp>
#include <types.hpp>
#include <hardware/gdt.hpp>
#include <hardware/idt.hpp>
#include <lib/log.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, u32 multibootMagic) {
    Terminal terminal;
    terminal.clear();

    log(0, "Initializing GDT");
    GDT gdt;

    int stack = 0;
    asm volatile("mov %%esp, %0" : "=r"(stack));
    setTSS(0x10, stack);

    log(0, "Loading interrupt manager");
    InterruptManager interruptManager;

    log(0, "Activating interrupts");
    interruptManager.activate();

    log(0, "Kernel loaded!");
    printk("Kernel loaded!");

    while (true) {
    }
}
