#include <Lib/String.hpp>
#include <MM/Heap.hpp>
#include <MM/PMM.hpp>

extern "C" void* bootPageDirectory;

VirtualMemoryManager::VirtualMemoryManager() {
    instance = this;
    currentDirectory = nullptr;

    auto* directory = (PageDirectory*) &bootPageDirectory;
    PageDirectoryEntry pde;
    memset(&pde, 0, sizeof(PageDirectoryEntry));
    pde.present = 1;
    pde.readWrite = 1;
    pde.pageSize = 0;
    pde.frame = virt2phys((u32) &bootPageDirectory) / PAGE_SIZE;
    directory->entries[1023] = pde;

    for (u32 i = KERNEL_HEAP_START; i < KERNEL_HEAP_START + KERNEL_HEAP_SIZE; i += PAGE_SIZE) {
        allocPage(getPageForAddress(i, true), false, true);
    }

    directory->entries[0].present = 1;
    directory->entries[0].readWrite = 1;
    directory->entries[0].isUser = 0;
    directory->entries[0].pageSize = 1;
    directory->entries[0].frame = 0;

    reloadCR3();
}

bool VirtualMemoryManager::allocPage(PageTableEntry* pte, bool kernel, bool writable) {
    void* block = PhysicalMemoryManager::instance->allocBlock();

    if (!block) {
        return false;
    }

    pte->frame = (u32) block / PAGE_SIZE;
    pte->present = 1;

    if (!kernel) {
        pte->isUser = 1;
    }

    if (writable) {
        pte->readWrite = 1;
    }

    return true;
}

void VirtualMemoryManager::freePage(PageTableEntry* pte) {
    void* ptr = (void*) (pte->frame * PAGE_SIZE);

    if (ptr) {
        PhysicalMemoryManager::instance->freeBlock(ptr);
    }

    pte->present = 0;
}

PageTableEntry* VirtualMemoryManager::getPageForAddress(u32 address, bool shouldCreate, bool write, bool user) {
    u32 pageDirIndex = PAGEDIR_INDEX(address);
    u32 pageTableIndex = PAGETBL_INDEX(address);

    auto* pageDir = (PageDirectory*) PAGE_DIRECTORY_ADDRESS;
    if (pageDir->entries[pageDirIndex].present == 0 && shouldCreate) {
        void* pageTablePhys = PhysicalMemoryManager::instance->allocBlock();

        memset(&pageDir->entries[pageDirIndex], 0, sizeof(PageDirectoryEntry));
        pageDir->entries[pageDirIndex].frame = (u32) pageTablePhys / PAGE_SIZE;
        if (write) {
            pageDir->entries[pageDirIndex].readWrite = 1;
        }

        if (user) {
            pageDir->entries[pageDirIndex].isUser = 1;
        }

        pageDir->entries[pageDirIndex].present = 1;

        auto* pageTableVirt = (PageTable*) getPageTableAddress(pageDirIndex);
        memset(pageTableVirt, 0, sizeof(PageTable));

        return &(pageTableVirt->entries[pageTableIndex]);
    }

    auto* pageTable = (PageTable*) getPageTableAddress(pageDirIndex);
    return &pageTable->entries[pageTableIndex];
}

void* VirtualMemoryManager::getPageTableAddress(u16 pageTableNumber) {
    u32 ret = PAGE_TABLE_ADDRESS;
    ret |= (pageTableNumber << PAGE_OFFSET_BITS);
    return (void*) ret;
}

PageDirectory* VirtualMemoryManager::createAddressSpace() {
    auto* directory = (PageDirectory*) PhysicalMemoryManager::instance->allocBlock();
    if (!directory) {
        return 0;
    }

    memset(directory, 0, sizeof(PageDirectory));
    return directory;
}

void VirtualMemoryManager::mapPage(void* physicalAddress, void* virtualAddress) {
    auto* pageDirectory = currentDirectory;

    auto* entry = &pageDirectory->entries[PAGE_DIRECTORY_INDEX((u32) virtualAddress)];

    if (entry->present != 1) {
        auto* table = (PageTable*) PhysicalMemoryManager::instance->allocBlock();
        if (!table) {
            return;
        }

        memset(table, 0, sizeof(PageTable));

        entry = &pageDirectory->entries[PAGE_DIRECTORY_INDEX((u32) virtualAddress)];

        entry->present = 1;
        entry->readWrite = 1;
        entry->frame = (u32) table;
    }
}

void* VirtualMemoryManager::virtualToPhysical(void* address) {
    u32 pageDirOffset = PAGEDIR_INDEX(address);
    u32 pageTableOffset = PAGETBL_INDEX((u32) address);
    u32 pageOffset = PAGEFRAME_INDEX(address);

    auto* pageTable = (PageTable*) (PAGE_TABLE_ADDRESS + (PAGE_SIZE * pageDirOffset));
    auto pageTableEntry = pageTable->entries[pageTableOffset];

    u32 physAddress = (pageTableEntry.frame * PAGE_SIZE) | pageOffset;

    return (void*) physAddress;
}

void VirtualMemoryManager::enablePaging() {
    asm volatile("mov %cr0, %eax;"
                 "or $0x80000000, %eax;"
                 "mov %eax, %cr0;");
}

bool VirtualMemoryManager::setCurrentDirectory(PageDirectory* directory) {
    if (!directory) {
        return false;
    }

    currentDirectory = directory;

    currentPageDirectoryBaseRegister = (u32) &directory->entries;
    asm("movl %0, %%cr3" : : "r"(currentPageDirectoryBaseRegister));
    return true;
}

void VirtualMemoryManager::reloadCR3() {
    asm volatile("movl %cr3,%eax");
    asm volatile("movl %eax,%cr3");
}

void VirtualMemoryManager::switchPageDirectory(u32 address) {
    //    printk("%d\n", address);
    asm volatile("mov %0, %%cr3" ::"r"(address));
}

u32 VirtualMemoryManager::getCR3() {
    u32 cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}
