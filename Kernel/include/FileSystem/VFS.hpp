#ifndef __VFS_HPP
#define __VFS_HPP

#include <Lib/Types.hpp>
#include <Driver/ATA.hpp>

class VirtualFileSystem {
public:
    VirtualFileSystem();
    ~VirtualFileSystem() = default;

    virtual int readFile(char* path, u8* buffer, u32 offset, int size);

    virtual bool fileExists(char* path);
    virtual bool directoryExists(char* path);

    virtual u32 getFileSize(char* path);
};

#endif
