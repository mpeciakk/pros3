#ifndef __HEAP_HPP
#define __HEAP_HPP

#include <types.hpp>
#include <lib/singleton.hpp>
#include <memory/vmm.hpp>

#define KERNEL_HEAP_START (u32) (KERNEL_VIRT_ADDR + 4 * 1024 * 1024)
#define KERNEL_HEAP_SIZE (u32) 16 * 1024 * 1024

#define PAGE_DIRECTORY_ADDRESS 0xFFFFF000

#define PAGEDIR_INDEX(addr) (((u32) addr) >> 22)
#define PAGETBL_INDEX(addr) ((((u32) addr) >> 12) & 0x3ff)
#define PAGEFRAME_INDEX(addr) (((u32) addr) & 0xfff)

struct MemoryChunk {
    MemoryChunk* next;
    MemoryChunk* prev;

    bool allocated;
    u32 size;
};

class MemoryManager : public Singleton<MemoryManager> {
public:
    MemoryManager(u32 start, u32 size);

    void* malloc(u32 size, u32* phys = nullptr);
    void* alignedMalloc(u32 size, u32 align, u32* phys = nullptr);
    void free(void* ptr);

private:
    MemoryChunk* first;

    u32 size;
    u32 usedMemory;

    void* internalMalloc(u32 size);
};

void* operator new(unsigned size);
void* operator new[](unsigned size);

void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif