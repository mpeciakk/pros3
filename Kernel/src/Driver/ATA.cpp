#include <Driver/ATA.hpp>
#include <Lib/Log.hpp>

ATA::ATA(u16 portBase, bool master)
    : dataPort(portBase), errorPort(portBase + 1), sectorCountPort(portBase + 2), lbaLowPort(portBase + 3), lbaMidPort(portBase + 4),
      lbaHighPort(portBase + 5), devicePort(portBase + 6), commandPort(portBase + 7), controlPort(portBase + 0x206) {
    bytesPerSector = 512;
    this->master = master;
}

void ATA::identify() {
    devicePort.write(master ? 0xA0 : 0xB0); // identify if device is master or slave
    sectorCountPort.write(0);               // clear sector count
    lbaLowPort.write(0);                    // clear lba
    lbaMidPort.write(0);                    // clear lba
    lbaHighPort.write(0);                   // clear lba
    commandPort.write(0xEC);                // send identify command

    u8 status = commandPort.read(); // read device status

    if (status == 0x00) {
        // no device on that bus
        return;
    }

    // is device busy?
    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
        status = commandPort.read();
    }

    if (status & 0x01) {
        log(2, "ATA Error");
    }

    for (u16 i = 0; i < 256; i++) {
        u16 data = dataPort.read();

#if 0
        char* text = "  \0";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;

        printf("%s", text);
#endif
    }
}

void ATA::read28(u32 sector, u8* buffer, u32 count) {
    if (sector > 0x0FFFFFFF) {
        return;
    }

    if (count > bytesPerSector) {
        return;
    }

    devicePort.write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.write(0);
    sectorCountPort.write(1);

    lbaLowPort.write(sector & 0x000000FF);
    lbaMidPort.write((sector & 0x0000FF00) >> 8);
    lbaHighPort.write((sector & 0x00FF0000) >> 16);
    commandPort.write(0x20);

    u8 status = commandPort.read();

    // is device busy?
    while (((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
        status = commandPort.read();
    }

    if (status & 0x01) {
        log(2, "ATA Error");
    }

    for (u32 i = 0; i < count; i += 2) {
        u16 readData = dataPort.read();

        buffer[i] = readData & 0x00FF;

        if (i + 1 < count) {
            buffer[i + 1] = (readData >> 8) & 0x00FF;
        }
    }

    for (u16 i = count + (count % 2); i < bytesPerSector; i += 2) {
        dataPort.read();
    }
}