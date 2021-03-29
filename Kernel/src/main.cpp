#include <Driver/ATA.hpp>
#include <FileSystem/FAT.hpp>
#include <FileSystem/PartitionTable.hpp>
#include <Hardware/GDT.hpp>
#include <Lib/Log.hpp>
#include <MM/Heap.hpp>
#include <MM/PMM.hpp>
#include <Multiboot.hpp>
#include <Tasking/Scheduler.hpp>
#include <Terminal.hpp>

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

    log(0, "Initializing interrupt manager");
    InterruptManager interruptManager;
    interruptManager.activate();

    u64 kernelSize = (u64) &kernelEnd - (u64) &kernelStart;
    printk("Loading Kernel at address %d with size of %dKB\n", (u32) &kernelStart, (u32) kernelSize / 1024);

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

    log(0, "Initializing physical memory manager with %dMB memory size", memorySize / 1024 / 1024);
    PhysicalMemoryManager pmm((u32) &kernelEnd + 0x1000, memorySize);

    for (memoryMap = (multiboot_mmap_entry*) phys2virt(mbi->mmap_addr); (u32) memoryMap < (phys2virt(mbi->mmap_addr) + mbi->mmap_length);
         memoryMap = (multiboot_mmap_entry*) ((u32) memoryMap + memoryMap->size + sizeof(memoryMap->size))) {

        if (memoryMap->type == MULTIBOOT_MEMORY_AVAILABLE || memoryMap->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
            pmm.setRegionFree(memoryMap->addr_low, memoryMap->len_low);
        }
    }

    pmm.setRegionUsed(0, 1 * 1024 * 1024);
    pmm.setRegionUsed(virt2phys(&kernelStart), virt2phys(&kernelSize));

    log(0, "%d allocation blocks, used blocks: %d, free blocks: %d", pmm.maxBlocks, pmm.usedBlocks, pmm.freeBlocksCount());

    log(0, "Free blocks: %d", pmm.freeBlocksCount());

    log(0, "Initializing virtual memory manager");
    VirtualMemoryManager vmm;

    log(0, "Free blocks: %d", pmm.freeBlocksCount());

    MemoryManager memoryManager(KERNEL_HEAP_START, KERNEL_HEAP_START + KERNEL_HEAP_SIZE);
    log(0, "Initialized heap on address 0x%x with size 0x%x", KERNEL_HEAP_START, KERNEL_HEAP_SIZE);

    log(0, "Free blocks: %d", pmm.freeBlocksCount());

    log(0, "Initializing process manager");
    ProcessManager processManager;

    log(0, "Initializing thread manager");
    ThreadManager threadManager;

    log(0, "Initializing process scheduler");
    ProcessScheduler processScheduler;

//    log(0, "Loading Kernel Tasking");
//    Process* kernelProcess = processManager.createKernelProcess();
//    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle, true));
//    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle2, true));
    //    kernelProcess->threads.pushBack(threadManager.createFromFunction(idle3, true));
//    kernelProcess->threads[0]->parent = kernelProcess;
//    kernelProcess->threads[1]->parent = kernelProcess;
//    processScheduler.addThread(kernelProcess->threads[0]);
//    processScheduler.addThread(kernelProcess->threads[1]);
    //    processScheduler.addThread(kernelProcess->threads[2]);

    log(0, "Initializing ATA Primary Master");
    ATA ata0m(0x1F0, true);
    ata0m.identify();

    log(0, "Initializing ATA Primary Slave");
    ATA ata0s(0x1F0, false);
    ata0s.identify();

    log(0, "Initializing FileSystem");
    MasterBootRecord mbr = PartitionTable::readPartitions(&ata0s);

    FATFileSystem fat(&ata0s, mbr.primaryPartition[0].startLba);

    log(0, "Kernel loaded!");
    printk("Kernel loaded!\n");

    log(0, "Free blocks: %d", pmm.freeBlocksCount());

//    u32 size = fat.getFileSize("/init.out");

//    u8* buffer = new u8[size];

//    log(0, "Buffer created!");

//    fat.readFile("/init.out", buffer);

//    log(0, "tak");

//    char* str = new char[8];

//    Process* process = processManager.createProcess("/init.out", true);

//    log(0, "Process created!");

//    if (Tasking != nullptr) {
//        log(0, "Launching init Tasking");

//        processScheduler.addThread(Tasking->threads[0]);
//    }

    while (true) {
    }
}
