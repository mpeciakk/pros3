#include <Lib/Memory.hpp>
#include <Lib/Panic.hpp>
#include <MM/Heap.hpp>
#include <MM/PMM.hpp>
#include <MM/VMM.hpp>

MemoryManager::MemoryManager(u32 start, u32 size) {
    instance = this;

    usedMemory = 0;
    this->size = size;

    if (size < sizeof(MemoryChunk)) {
        first = nullptr;
    } else {
        first = (MemoryChunk*) start;

        first->allocated = false;
        first->prev = nullptr;
        first->next = nullptr;
        first->size = size - sizeof(MemoryChunk);
    }
}

void* MemoryManager::malloc(u32 size, u32* phys) {
    void* address = internalMalloc(size);

    if (phys != nullptr) {
        auto* page = VirtualMemoryManager::instance->getPageForAddress((u32) address, false);
        *phys = (page->frame * PAGE_SIZE) + ((u32) address & 0xFFF);
    }

    return address;
}

void* MemoryManager::internalMalloc(u32 size) {
    if (usedMemory + size + sizeof(MemoryChunk) >= this->size) {
        KPANIC("Out of memory");

        return nullptr;
    }

    MemoryChunk* result = nullptr;

    for (MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next) {
        if (chunk->size > size && !chunk->allocated) {
            result = chunk;
        }
    }

    if (result == nullptr) {
        KPANIC("Out of memory");

        return nullptr;
    }

    if (result->size >= size + sizeof(MemoryChunk) + 1) {
        auto* temp = (MemoryChunk*) ((u32) result + sizeof(MemoryChunk) + size);

        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;

        if (temp->next != nullptr) {
            temp->next->prev = temp;
        }

        result->size = size;
        result->next = temp;
    }

    result->allocated = true;

    usedMemory += size + sizeof(MemoryChunk);

    return (void*) (((u32) result) + sizeof(MemoryChunk));
}

void* MemoryManager::alignedMalloc(u32 size, u32 align, u32* physReturn) {
    void* ptr = nullptr;

    if ((align & (align - 1)) != 0) {
        return nullptr;
    }

    if (align && size) {
        u32 hdrSize = sizeof(u16) + (align - 1);

        u32 phys = 0;
        void* p = malloc(size + hdrSize, &phys);

        if (p) {
            ptr = (void*) (((u32) p + sizeof(u16) + ((align) -1)) & (~(align) -1));
            if (phys) {
                *physReturn = (u32) VirtualMemoryManager::instance->virtualToPhysical(ptr);
            }

            *((u16*) ptr - 1) = (u16)((u32) ptr - (u32) p);
        }
    }
    return ptr;
}

void MemoryManager::free(void* ptr) {
    auto* chunk = (MemoryChunk*) ((u32) ptr - sizeof(MemoryChunk));

    chunk->allocated = false;

    if (chunk->prev != nullptr && chunk->prev->allocated) {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);

        if (chunk->next != nullptr) {
            chunk->next->prev = chunk->prev;
        }

        chunk = chunk->prev;
    }

    if (chunk->next != nullptr && !chunk->next->allocated) {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;

        if (chunk->next != nullptr) {
            chunk->next->prev = chunk->prev;
        }
    }
}

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void* operator new(size_t size, void* ptr) {
    return ptr;
}

void* operator new[](size_t size, void* ptr) {
    return ptr;
}

void operator delete(void* ptr) {
    free(ptr);
}

void operator delete(void* ptr, size_t size) {
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr, size_t size) {
    free(ptr);
}