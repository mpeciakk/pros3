#ifndef __PMM_HPP
#define __PMM_HPP

#define BLOCKS_PER_BYTE 8
#define BLOCK_SIZE 4096
#define BLOCK_ALIGN BLOCK_SIZE

#define phys2virt(x) ((u32)(x) + (u32) 3 * 1024 * 1024 * 1024)

#include <Lib/Types.hpp>
#include <Lib/Singleton.hpp>

class PhysicalMemoryManager : public Singleton<PhysicalMemoryManager> {
public:
    u32 maxBlocks;
    u32 usedBlocks;

    PhysicalMemoryManager(u32 memoryMap, u32 memorySize);
    ~PhysicalMemoryManager() = default;

    void setRegionFree(u32 start, u32 size);
    void setRegionUsed(u32 start, u32 size);

    void* allocBlock();
    void* allocBlocks(u32 count);
    void freeBlock(void* ptr);

    u32 freeBlocksCount();

private:
    u32* memoryMap;
    u32 memorySize;

    int firstFree();
    int firstFree(u32 size);

    void set(int bit);
    void unset(int bit);
    bool test(int bit);
};

u32 pageRoundUp(u32 address);

#endif