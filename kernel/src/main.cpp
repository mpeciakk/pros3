#include <driver/ata.hpp>
#include <filesystem/fat.hpp>
#include <filesystem/partitiontable.hpp>
#include <hardware/gdt.hpp>
#include <hardware/idt.hpp>
#include <lib/log.hpp>
#include <memory/heap.hpp>
#include <memory/pmm.hpp>
#include <multiboot.hpp>
#include <process/scheduler.hpp>
#include <process/thread.hpp>
#include <terminal.hpp>
#include <types.hpp>

extern "C" u32* kernelStart;
extern "C" u32* kernelEnd;

void idle() {
    while (true) {
        printf("a");
    }
}

void idle2() {
    while (true) {
        printf("b");
    }
}

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

    printk("Loading kernel at address %d with size of %dKB\n", (u32) &kernelStart, (u32) kernelSize / 1024);

    u32 memorySize = 0;
    multiboot_memory_map_t* memoryMap;

    printk("Detected memory:\n");
    for (memoryMap = (multiboot_mmap_entry*) phys2virt(mbi->mmap_addr); (u32) memoryMap < (phys2virt(mbi->mmap_addr) + mbi->mmap_length);
         memoryMap = (multiboot_mmap_entry*) ((u32) memoryMap + memoryMap->size + sizeof(memoryMap->size))) {
        printk("Address low = %d, address high = %d "
               "length low = %d, length high = %d, type = %d\n",
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

    log(0, "Loading process manager");
    ProcessManager processManager;

    log(0, "Loading thread manager");
    ThreadManager threadManager;

    log(0, "Loading process sheduler");
    ProcessScheduler processScheduler;

    log(0, "Loading kernel process");
    Process* kernelProcess = processManager.createKernelProcess();
//    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle, true));
//    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle2, true));
    //    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle3, true));
//    kernelProcess->threads[0]->parent = kernelProcess;
//    kernelProcess->threads[1]->parent = kernelProcess;
//    processScheduler.addThread(kernelProcess->threads[0]);
//    processScheduler.addThread(kernelProcess->threads[1]);
    //    processScheduler.addThread(kernelProcess->threads[2]);

    log(0, "Loading ATA Primary Master");
    ATA ata0m(0x1F0, true);
    ata0m.identify();

    log(0, "Loading ATA Primary Slave");
    ATA ata0s(0x1F0, false);
    ata0s.identify();

    log(0, "Loading filesystem");
    MasterBootRecord mbr = PartitionTable::readPartitions(&ata0s);

    FATFileSystem fat(&ata0s, mbr.primaryPartition[0].startLba);

    log(0, "Kernel loaded!");
    printk("Kernel loaded!\n");

//    u32 size = fat.getFileSize("/init.out");

//    u8* buffer = new u8[size];

//    log(0, "Buffer created!");

//    fat.readFile("/init.out", buffer);

//    log(0, "tak");

    Process* process = processManager.createProcess("/init.out", true);

//    if (process != nullptr) {
//        log(0, "Launching init process");

//        processScheduler.addThread(process->threads[0]);
//    }

    while (true) {
    }
}
