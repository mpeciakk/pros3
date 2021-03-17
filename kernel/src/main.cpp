#include <hardware/gdt.hpp>
#include <hardware/idt.hpp>
#include <lib/log.hpp>
#include <memory/heap.hpp>
#include <memory/pmm.hpp>
#include <multiboot.hpp>
#include <terminal.hpp>
#include <types.hpp>

extern "C" u32* kernelStart;
extern "C" u32* kernelEnd;

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

    log(0, "Initializing physical memory manager");
    u64 kernelSize = (u64) &kernelEnd - (u64) &kernelStart;

    printk("Kernel loaded at %d with size of %dKB\n", &kernelStart, kernelSize / 1024);

    u32 memorySize = 0;
    multiboot_memory_map_t* memoryMap;

    printk("Detected memory:\n");
    for (memoryMap = (multiboot_mmap_entry*) phys2virt(mbi->mmap_addr); (u32) memoryMap < (phys2virt(mbi->mmap_addr) + mbi->mmap_length);
         memoryMap = (multiboot_mmap_entry*) ((u32) memoryMap + memoryMap->size + sizeof(memoryMap->size))) {
        printk("Address low = 0x%x, address high = 0x%x "
               "length low = 0x%x, length high = 0x%x, type = 0x%x\n",
               memoryMap->addr_low, memoryMap->addr_high, memoryMap->len_low, memoryMap->len_high, memoryMap->type);

        memorySize += memoryMap->len_low | ((u64) memoryMap->len_high) << 32;
    }

    PhysicalMemoryManager pmm((u32) &kernelEnd + 0x1000, memorySize);

    log(0, "PMM initialized with %dMB physical memory", memorySize / 1024 / 1024);

    for (memoryMap = (multiboot_mmap_entry*) phys2virt(mbi->mmap_addr); (u32) memoryMap < (phys2virt(mbi->mmap_addr) + mbi->mmap_length);
         memoryMap = (multiboot_mmap_entry*) ((u32) memoryMap + memoryMap->size + sizeof(memoryMap->size))) {

        if (memoryMap->type == 1 || memoryMap->type == 2) {
            pmm.initRegion(memoryMap->addr_low, memoryMap->len_low);
        }
    }

    pmm.deinitRegion(0, 0x1000 + kernelSize + (memorySize / BLOCK_SIZE / BLOCKS_PER_BYTE));

    log(0, "%d allocation blocks, used blocks: %d, free blocks: %d", pmm.maxBlocks, pmm.usedBlocks, pmm.freeBlocksCount());

    log(0, "Loading virtual memory manager");
    VirtualMemoryManager vmm;

    MemoryManager memoryManager(KERNEL_HEAP_START, KERNEL_HEAP_START + KERNEL_HEAP_SIZE);
    log(0, "Initialized heap on address 0x%x with size 0x%x", KERNEL_HEAP_START, KERNEL_HEAP_SIZE);

    log(0, "Kernel loaded!");
    printk("Kernel loaded!");

    while (true) {
    }
}
