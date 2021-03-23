#ifndef __FAT_HPP
#define __FAT_HPP

#include <filesystem/virtualfilesystem.hpp>
#include <lib/linkedlist.hpp>
#include <types.hpp>

#define CLUSTER_FREE 0x00
#define CLUSTER_END 0x0FFFFFF8

struct BiosParameterBlock {
    u8 jump[3];
    u8 softwareName[8];
    u16 bytesPerSector;
    u8 sectorsPerCluster;
    u16 reservedSectors;
    u8 fatCopies;
    u16 rootDirectoryEntries; // for FAT32 should always be 0
    u16 totalSectors;
    u8 mediaType;
    u16 fatSectorCount; // for FAT32 should always be 0
    u16 sectorsPerTrack;
    u16 headCount;
    u32 hiddenSectors;
    u32 totalSectorCount;

    u32 fatSize;
    u16 flags;
    u16 fatVersion;
    u32 rootCluster;
    u16 fatInfo;
    u16 backupSector;
    u8 reserved0[12];
    u8 driveNumber;
    u8 reserved;
    u8 bootSignal;
    u32 volumeId;
    u8 volumeLabel[11];
    u8 fatLabelType[8];
} __attribute__((packed));

struct DirectoryEntryFat {
    char name[11];

    u8 attributes;
    u8 reserved;
    u8 createTimeTenth;
    u16 createTime;
    u16 createDate;
    u16 accessTime;
    u16 firstClusterHigh;
    u16 writeTime;
    u16 writeDate;
    u16 firstClusterLow;
    u32 size;
} __attribute__((packed));

class FATFileSystem : public VirtualFileSystem {
public:
    FATFileSystem(ATA* disk, u32 partitionOffset);

    int readFile(char* path, u8* buffer, u32 offset = 0, int size = -1) override;
    bool fileExists(char *path) override;
    bool directoryExists(char *path) override;
    u32 getFileSize(char *path) override;

    static char* convertName(const char name[11]);

private:
    BiosParameterBlock bpb;
    u32 partitionOffset;
    u32 dataStart;

    ATA* disk;

    inline u32 clusterToLBA(u32 cluster) const;
    u32 getNextCluster(u32 cluster);

    Vector<DirectoryEntryFat*>* getEntries(u32 cluster);
    Vector<DirectoryEntryFat*>* getEntries(char* path);
    DirectoryEntryFat* getEntryInCluster(char* name, u32 cluster);
    DirectoryEntryFat* getEntry(char* path);
    void readClusterChain(u32 firstCluster, u8* buffer, int fileSize);
};

#endif
