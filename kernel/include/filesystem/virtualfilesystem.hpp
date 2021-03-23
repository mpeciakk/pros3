#ifndef __VIRTUALFILESYSTEM_HPP
#define __VIRTUALFILESYSTEM_HPP

#include <driver/ata.hpp>
#include <types.hpp>

class VirtualFileSystem {
public:
    VirtualFileSystem();

    virtual int readFile(char* path, u8* buffer, u32 offset = 0, int size = -1);

    virtual bool fileExists(const char* path);
    virtual bool directoryExists(const char* path);

    virtual u32 getFileSize(const char* path);
};

#endif
