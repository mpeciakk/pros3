#include <filesystem/virtualfilesystem.hpp>

VirtualFileSystem::VirtualFileSystem() { }

int VirtualFileSystem::readFile(char* path, u8* buffer, u32 offset, int size) {
    return 0;
}

bool VirtualFileSystem::fileExists(char* path) {
    return false;
}

bool VirtualFileSystem::directoryExists(char* path) {
    return false;
}

u32 VirtualFileSystem::getFileSize(char* path) {
    return 0;
}
