#ifndef __VMM_HPP
#define __VMM_HPP

#include <Lib/Singleton.hpp>
#include <Lib/Types.hpp>

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR 1024

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) ((((u32) x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PAGEDIR_INDEX(addr) (((u32) addr) >> 22)
#define PAGEFRAME_INDEX(addr) (((u32) addr) & 0xfff)

#define PAGE_SIZE 4096

#define virt2phys(x) ((u32) (x) - (u32) 3 * 1024 * 1024 * 1024)

#define KERNEL_VIRT_ADDR (u32) 3 * 1024 * 1024 * 1024
#define USER_STACK_SIZE 32 * 1024
#define USER_STACK_TOP (KERNEL_VIRT_ADDR)
#define USER_STACK (USER_STACK_TOP - USER_STACK_SIZE)

#define KERNEL_PTNUM 768
#define PAGE_TABLE_ADDRESS 0xFFC00000
#define PAGE_DIRECTORY_ADDRESS 0xFFFFF000

#define PAGE_OFFSET_BITS 12

struct PageDirectoryEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 reserved : 1;
    u32 pageSize : 1;
    u32 ignored : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTableEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 dirty : 1;
    u32 reserved : 1;
    u32 global : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTable {
    PageTableEntry entries[PAGES_PER_TABLE];

    inline PageTableEntry* lookupEntry(u32 address) {
        return &entries[PAGE_TABLE_INDEX(address)];
    }
};

struct PageDirectory {
    PageDirectoryEntry entries[PAGES_PER_DIR];

    inline PageDirectoryEntry* lookupEntry(u32 address) {
        return &entries[PAGE_TABLE_INDEX(address)];
    }
};

class VirtualMemoryManager : public Singleton<VirtualMemoryManager> {
public:
    PageDirectory* currentDirectory;

    VirtualMemoryManager();
    ~VirtualMemoryManager() = default;

    bool allocPage(PageTableEntry* pte, bool kernel, bool writable);
    void freePage(PageTableEntry* pte);

    PageTableEntry* getPageForAddress(u32 address, bool shouldCreate, bool write = true, bool user = false);
    void* getPageTableAddress(u16 pageTableNumber);

    PageDirectory* createAddressSpace();
    void mapPage(void* physicalAddress, void* virtualAddress);

    void* virtualToPhysical(void* pVoid);

    void enablePaging();
    bool setCurrentDirectory(PageDirectory* directory);
    void reloadCR3();
    void switchPageDirectory(u32 address);
    u32 getCR3();

private:
    u32 currentPageDirectoryBaseRegister;
};

#endif