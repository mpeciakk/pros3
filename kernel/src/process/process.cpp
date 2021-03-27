#include <filesystem/fat.hpp>
#include <hardware/idt.hpp>
#include <lib/log.hpp>
#include <memory/heap.hpp>
#include <memory/pmm.hpp>
#include <process/elf.hpp>
#include <process/process.hpp>

extern "C" void* bootPageDirectory;

ProcessManager::ProcessManager() { }

Process* ProcessManager::createKernelProcess() {
    Process* process = new Process();

    process->name = "Kernel";
    process->id = currentPID++;
    process->pageDirPhys = virt2phys(&bootPageDirectory);

    processes.pushBack(process);

    return process;
}

Process* ProcessManager::createProcess(char* fileName, bool isKernel) {
    if (!FATFileSystem::instance->fileExists(fileName)) {
        log(2, "File does not exists");
        return nullptr;
    }

    u32 fileSize = FATFileSystem::instance->getFileSize(fileName);

    printk("%d\n", fileSize);

    u8* buffer = new u8[fileSize];
    if (FATFileSystem::instance->readFile(fileName, buffer) != 0) {
        log(2, "Couldn't read file");

        delete[] buffer;
        return nullptr;
    }

    printk("-2\n");

    ElfHeader* header = (ElfHeader*) buffer;
    if (!(header->ident[0] == ELFMAG0 && header->ident[1] == ELFMAG1 && header->ident[2] == ELFMAG2 && header->ident[3] == ELFMAG3) ||
        header->type != 2) {
        log(2, "Wrong elf signature");

        delete[] buffer;
        return 0;
    }

    u32 pageDirPhys = 0;
    PageDirectory* pageDirectory = (PageDirectory*) MemoryManager::instance->alignedMalloc(sizeof(PageDirectory), sizeof(PageDirectory), &pageDirPhys);
    VirtualMemoryManager::instance->switchPageDirectory(pageDirPhys);

    pageDirectory->entries[KERNEL_PTNUM] = ((PageDirectory*) &bootPageDirectory)->entries[KERNEL_PTNUM];

    for (u32 i = 0; i < KERNEL_HEAP_SIZE / 4 * 1024 * 1024; i++) {
        pageDirectory->entries[KERNEL_PTNUM + i + 1] = ((PageDirectory*) &bootPageDirectory)->entries[KERNEL_PTNUM + i + 1];
    }

    PageDirectoryEntry lastPDE;
    lastPDE.frame = pageDirPhys / PAGE_SIZE;
    lastPDE.readWrite = 1;
    lastPDE.pageSize = 0;
    lastPDE.present = 1;
    pageDirectory->entries[1023] = lastPDE;

    InterruptManager::instance->deactivate();

    u32 oldCR3 = VirtualMemoryManager::instance->getCR3();
    VirtualMemoryManager::instance->switchPageDirectory(pageDirPhys);

    Process* process = new Process();

    ElfProgramHeader* programHeader = (ElfProgramHeader*) (buffer + header->phoff);

    for (int i = 0; i < header->phnum; i++, programHeader++) {
        if (programHeader->type == 1) {
            for (u32 j = 0; j < programHeader->memsz; j += PAGE_SIZE) {
                VirtualMemoryManager::instance->allocPage(
                    VirtualMemoryManager::instance->getPageForAddress(programHeader->vaddr + j, true, true, !isKernel), isKernel, true);

                if (programHeader->vaddr < process->executable.memoryBase || process->executable.memoryBase == 0) {
                    process->executable.memoryBase = programHeader->vaddr;
                }

                if (programHeader->vaddr + programHeader->memsz - process->executable.memoryBase > process->executable.memorySize) {
                    process->executable.memorySize = programHeader->vaddr + programHeader->memsz - process->executable.memoryBase;
                }
            }
        }
    }

    VirtualMemoryManager::instance->reloadCR3();

    programHeader = (ElfProgramHeader*) (buffer + header->phoff);

    for (int i = 0; i < header->phnum; i++, programHeader++) {
        if (programHeader->type == 1) {
            memcpy((void*) programHeader->vaddr, buffer + programHeader->offset, programHeader->memsz);
        }
    }

    process->id = currentPID++;
    process->pageDirPhys = pageDirPhys;
    process->isUserspace = !isKernel;
    process->threads.pushBack(ThreadManager::instance->createFromFunction((void (*)()) header->entry, isKernel));

    Thread* mainThread = process->threads[0];

    for (u32 i = (u32) mainThread->userStack; i < ((u32) mainThread->userStack + mainThread->userStackSize); i += PAGE_SIZE) {
        VirtualMemoryManager::instance->allocPage(VirtualMemoryManager::instance->getPageForAddress(i, true, true, !isKernel), isKernel,
                                                  true);
    }

    for (u32 i = 0; i < PROC_USER_HEAP_SIZE; i += PAGE_SIZE) {
        u32 addr = pageRoundUp(process->executable.memoryBase + process->executable.memorySize) + i;
        VirtualMemoryManager::instance->allocPage(VirtualMemoryManager::instance->getPageForAddress(addr, true, true, !isKernel),
                                                  isKernel, true);
    }

    process->heap.heapStart = pageRoundUp(process->executable.memoryBase + process->executable.memorySize);
    process->heap.heapEnd = process->heap.heapStart + PROC_USER_HEAP_SIZE;

    mainThread->parent = process;

    VirtualMemoryManager::instance->switchPageDirectory(oldCR3);

    process->name = fileName;

    processes.pushBack(process);
}
