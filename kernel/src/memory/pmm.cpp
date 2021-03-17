#include <lib/string.hpp>
#include <memory/pmm.hpp>
#include <panic.hpp>

PhysicalMemoryManager::PhysicalMemoryManager(u32 memoryMap, u32 memorySize) {
    instance = this;

    this->memoryMap = (u32*) memoryMap;
    this->memorySize = memorySize;
    this->maxBlocks = (memorySize * 1024) / BLOCK_SIZE;
    this->usedBlocks = maxBlocks;

    memset(this->memoryMap, 0xFF, maxBlocks / BLOCKS_PER_BYTE);
}

void PhysicalMemoryManager::initRegion(u32 start, u32 size) {
    int align = start / BLOCK_SIZE;
    int blocks = size / BLOCK_SIZE;

    for (; blocks >= 0; blocks--) {
        unset(align++);
        usedBlocks--;
    }

    set(0);
}

void PhysicalMemoryManager::deinitRegion(u32 start, u32 size) {
    int align = start / BLOCK_SIZE;
    int blocks = size / BLOCK_SIZE;

    for (; blocks >= 0; blocks--) {
        set(align++);
        usedBlocks++;
    }
}

void* PhysicalMemoryManager::allocBlock() {
    return allocBlocks(1);
}

void* PhysicalMemoryManager::allocBlocks(u32 count) {
    if (freeBlocksCount() <= count) {
        KPANIC("Out of memory");
        return nullptr;
    }

    int frame = firstFree(count);

    if (frame == -1) {
        KPANIC("Out of memory");
        return nullptr;
    }

    for (u32 i = 0; i < count; i++) {
        set(frame + i);
    }

    u32 address = frame * BLOCK_SIZE;
    usedBlocks += count;

    return (void*) address;
}

void PhysicalMemoryManager::freeBlock(void* ptr) {
    u32 address = (u32) ptr;
    int frame = address / BLOCK_SIZE;

    unset(frame);

    usedBlocks--;
}

int PhysicalMemoryManager::firstFree() {
    for (u32 i = 0; i < maxBlocks / 32; i++) {
        if (memoryMap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;

                if (!(memoryMap[i] & bit)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }

    return -1;
}

int PhysicalMemoryManager::firstFree(u32 size) {
    if (size == 0) {
        return -1;
    }

    if (size == 1) {
        return firstFree();
    }

    for (u32 i = 0; i < maxBlocks / 32; i++) {
        if (memoryMap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;
                if (!(memoryMap[i] & bit)) {
                    int startingBit = i * 32;
                    startingBit += bit;

                    u32 free = 0;
                    for (u32 count = 0; count <= size; count++) {
                        if (!test(startingBit + count)) {
                            free++;
                        }

                        if (free == size) {
                            return i * 4 * 8 + j;
                        }
                    }
                }
            }
        }
    }

    return -1;
}

void PhysicalMemoryManager::set(int bit) {
    memoryMap[bit / 32] |= (1 << (bit % 32));
}

void PhysicalMemoryManager::unset(int bit) {
    memoryMap[bit / 32] &= ~(1 << (bit % 32));
}

bool PhysicalMemoryManager::test(int bit) {
    return memoryMap[bit / 32] & (1 << (bit % 32));
}

u32 PhysicalMemoryManager::freeBlocksCount() {
    return maxBlocks - usedBlocks;
}
