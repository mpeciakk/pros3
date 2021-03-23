#include <filesystem/fat.hpp>
#include <lib/log.hpp>

FATFileSystem::FATFileSystem(ATA* disk, u32 partitionOffset) {
    this->disk = disk;
    this->partitionOffset = partitionOffset;

    disk->read28(partitionOffset, (u8*) &bpb, sizeof(BiosParameterBlock));

    dataStart = partitionOffset + bpb.reservedSectors + (bpb.fatSize * bpb.fatCopies) + bpb.rootDirectoryEntries;

#if 0
    printk("Software: %s\n", bpb.softwareName);
    printk("Volume label: %s\n", bpb.volumeLabel);
    printk("Bytes per sector: %d\n", bpb.bytesPerSector);
    printk("Sectors per cluster: %d\n", bpb.sectorsPerCluster);
    printk("Sector count: %d\n", bpb.totalSectorCount);
    printk("FAT copies: %d\n", bpb.fatCopies);
    printk("FAT size: %d\n", bpb.fatSize);
#endif
}

u32 FATFileSystem::clusterToLBA(u32 cluster) const {
    return dataStart + cluster * bpb.sectorsPerCluster - (2 * bpb.sectorsPerCluster);
}

char* FATFileSystem::convertName(const char name[11]) {
    u32 length = 0;

    for (int i = 0; i < 12; ++i) {
        if (name[i] != ' ') {
            length++;
        }
    }

    char* fileName = new char[length];

    u32 i = 0;
    for (int j = 0; j < 11; ++j) {
        if (j == 8 && name[j + 1] != ' ') {
            fileName[i++] = '.';
        }

        if (name[j] != ' ') {
            fileName[i++] = name[j];
        }
    }

    // convert string to lowercase
    for (char* p = fileName; *p; ++p) {
        *p = *p > 0x40 && *p < 0x5b ? *p | 0x60 : *p;
    }

    return fileName;
}

u32 FATFileSystem::getNextCluster(u32 cluster) {
    u8* fatBuffer = new u8[512];

    u32 fatSectorForCurrentCluster = cluster / (512 / sizeof(u32));
    disk->read28(partitionOffset + bpb.reservedSectors + fatSectorForCurrentCluster, fatBuffer, 512);

    u32 fatOffsetInSectorForCurrentCluster = cluster % (512 / sizeof(u32));
    cluster = ((u32*) fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;

    delete[] fatBuffer;

    return cluster;
}

Vector<DirectoryEntryFat*>* FATFileSystem::getEntries(u32 cluster) {
    Vector<DirectoryEntryFat*>* results = new Vector<DirectoryEntryFat*>();
    if (cluster == 0) {
        cluster = bpb.rootCluster;
    }

    u32 sector = clusterToLBA(cluster);

    while ((cluster != CLUSTER_FREE) && (cluster < CLUSTER_END)) {
        for (u32 i = 0; i < bpb.sectorsPerCluster; i++) {
            u8* buffer = new u8[bpb.bytesPerSector];

            this->disk->read28(sector + i, buffer, bpb.bytesPerSector);

            for (u32 j = 0; j < (bpb.bytesPerSector / sizeof(DirectoryEntryFat)); j++) {
                DirectoryEntryFat* entry = (DirectoryEntryFat*) (buffer + j * sizeof(DirectoryEntryFat));

                if (entry->name[0] == 0x00) { // end of entries
                    return results;
                }

                if (entry->name[0] == 0x2E) { // . or .. entry
                    continue;
                }

                if ((entry->attributes & 0x0F) == 0x0F) { // not file nor folder
                    continue;
                }

                DirectoryEntryFat* newEntry = new DirectoryEntryFat();
                memcpy(newEntry, entry, sizeof(DirectoryEntryFat));

                results->pushBack(newEntry);
            }

            delete[] buffer;
        }

        cluster = getNextCluster(cluster);
    }

    return results;
}

DirectoryEntryFat* FATFileSystem::getEntryInCluster(char* name, u32 cluster) {
    Vector<DirectoryEntryFat*>* entries = getEntries(cluster);

    for (int i = 0; i < entries->size(); ++i) {
        if (strcmp(convertName(entries->operator[](i)->name), name) == 0) {
            return entries->operator[](i);
        }
    }

    return nullptr;
}

Vector<DirectoryEntryFat*>* FATFileSystem::getEntries(char* path) {
    Vector<DirectoryEntryFat*>* result = new Vector<DirectoryEntryFat*>();

    Vector<char*> parts = split(path, '/');

    if (strcmp(path, "/") == 0) {
        return getEntries((u32) 0);
    }

    u32 cluster = 0;
    for (int i = 0; i < parts.size() + 1; i++) {
        // if it's final destination read all entries in it
        if (i == parts.size()) {
            result = getEntries(cluster);

            break;
        }

        if (strcmp(parts[i], "") == 0) {
            cluster = bpb.rootCluster;
        } else {
            DirectoryEntryFat* entry = getEntryInCluster(parts[i], cluster);

            if (entry == nullptr) {
                delete entry;

                break;
            }

            cluster = ((u32) entry->firstClusterLow) | (((u32) entry->firstClusterHigh) << 16);

            delete entry;
        }
    }

    return result;
}

DirectoryEntryFat* FATFileSystem::getEntry(char* path) {
    Vector<char*> parts = split(path, '/');

    u32 amountOfDelims = 0;
    u32 lastOccurrence = 0;
    while (true) {
        amountOfDelims++;

        int temp = indexOf(path, '/', amountOfDelims);

        if (temp < 0) {
            break;
        }

        lastOccurrence = (u32) temp;
    }

    u32 len = strlen(path);

    char* directoryPath = new char[(lastOccurrence == 0 ? 1 : lastOccurrence) + 1];
    memcpy(directoryPath, path, lastOccurrence);

    char* filename = new char[len - lastOccurrence];
    memcpy(filename, path + lastOccurrence + 1, len - lastOccurrence);

    if (strcmp(directoryPath, "") == 0) {
        directoryPath[0] = '/';
    }

    Vector<DirectoryEntryFat*>* entries = getEntries(directoryPath);

    for (int i = 0; i < entries->size(); ++i) {
        if (strcmp(convertName(entries->operator[](i)->name), filename) == 0) {
            delete[] filename;
            delete[] directoryPath;

            return entries->operator[](i);
        }
    }

    delete[] filename;
    delete[] directoryPath;

    return nullptr;
}

void FATFileSystem::readClusterChain(u32 firstCluster, u8* buffer, int fileSize) {
    u8* copyBuffer = buffer;
    u8 tempBuffer[512];

    u32 nextFileCluster = firstCluster;
    while (fileSize > 0) {
        u32 fileSector = dataStart + bpb.sectorsPerCluster * (nextFileCluster - 2);

        int sectorOffset = 0;
        for (; fileSize > 0; fileSize -= 512) {
            disk->read28(fileSector + sectorOffset, tempBuffer, fileSize < 512 ? fileSize : 512);

            memcpy(copyBuffer, tempBuffer, fileSize < 512 ? fileSize : 512);

            copyBuffer += 512;

            if (++sectorOffset > bpb.sectorsPerCluster) {
                break;
            }
        }

        nextFileCluster = getNextCluster(nextFileCluster);
    }

    copyBuffer[fileSize] = '\0';
}

int FATFileSystem::readFile(char* path, u8* buffer, u32 offset, int size) {
    DirectoryEntryFat* entry = getEntry(path);

    if (entry == nullptr) {
        return -1;
    }

    if (entry->attributes & 0x10) {
        delete entry;
        return -1;
    }

    u32 cluster = ((u32) entry->firstClusterLow) | (((u32) entry->firstClusterHigh) << 16);
    size = size == -1 ? entry->size : size;

    readClusterChain(cluster, buffer, size);

    delete entry;
    return 0;
}

bool FATFileSystem::fileExists(char* path) {
    DirectoryEntryFat* entry = getEntry(path);

    if (entry == nullptr) {
        return false;
    }

    bool exists = !(entry->attributes & 0x10);

    delete entry;
    return exists;
}

bool FATFileSystem::directoryExists(char* path) {
    DirectoryEntryFat* entry = getEntry(path);

    if (entry == nullptr) {
        return false;
    }

    bool exists = entry->attributes & 0x10;

    delete entry;
    return exists;
}

u32 FATFileSystem::getFileSize(char* path) {
    DirectoryEntryFat* entry = getEntry(path);

    if (entry == nullptr) {
        return 0;
    }

    u32 size = entry->size;

    delete entry;
    return size;
}
