#ifndef __PARTITIONTABLE_HPP
#define __PARTITIONTABLE_HPP

#include <filesystem/virtualfilesystem.hpp>

struct PartitionTableEntry {
    u8 bootable;

    u8 startHead;
    u8 startSector : 6;
    u16 startCylinder : 10;

    u8 partitionId;

    u8 endHead;
    u8 endSector : 6;
    u16 endCylinder : 10;

    u32 startLba;
    u32 length;
} __attribute__((packed));

struct MasterBootRecord {
    u8 bootloader[440];
    u32 signature;
    u16 unused;

    PartitionTableEntry primaryPartition[4];

    u16 magicNumber;
} __attribute__((packed));

class PartitionTable {
public:
    PartitionTable() = default;

    static MasterBootRecord readPartitions(ATA* disk);
};

#endif
