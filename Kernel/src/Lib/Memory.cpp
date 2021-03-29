#include <Lib/Memory.hpp>
#include <MM/Heap.hpp>

void* malloc(u32 size) {
    return MemoryManager::instance->malloc(size);
}

void free(void* ptr) {
    MemoryManager::instance->free(ptr);
}