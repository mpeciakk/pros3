#ifndef __ATA_HPP
#define __ATA_HPP

#include <hardware/port.hpp>
#include <types.hpp>

class ATA {
public:
    ATA(u16 portBase, bool master);

    void identify();
    void read28(u32 sector, u8* buffer, u32 count);

private:
    Port16Bit dataPort;
    Port8Bit errorPort;
    Port8Bit sectorCountPort;
    Port8Bit lbaLowPort;
    Port8Bit lbaMidPort;
    Port8Bit lbaHighPort;
    Port8Bit devicePort;
    Port8Bit commandPort;
    Port8Bit controlPort;

    u16 bytesPerSector;
    bool master;
};

#endif
