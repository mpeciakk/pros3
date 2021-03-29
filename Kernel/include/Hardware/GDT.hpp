#ifndef __GDT_HPP
#define __GDT_HPP

#include <Lib/Types.hpp>

#define MAX_DESCRIPTORS 6

void setTSS(u16 kernelSS, u16 kernelESP);

class GDT {
public:
    GDT();
    ~GDT() = default;

    void installTSS(u16 kernelSS, u16 kernelESP);

    struct TSSEntry {
        u32 prevTss;
        u32 esp0;
        u32 ss0;
        u32 esp1;
        u32 ss1;
        u32 esp2;
        u32 ss2;
        u32 cr3;
        u32 eip;
        u32 eflags;
        u32 eax;
        u32 ecx;
        u32 edx;
        u32 ebx;
        u32 esp;
        u32 ebp;
        u32 esi;
        u32 edi;
        u32 es;
        u32 cs;
        u32 ss;
        u32 ds;
        u32 fs;
        u32 gs;
        u32 ldt;
        u16 trap;
        u16 iomap;
    } __attribute__((packed));
private:
    struct Descriptor {
        u16 limit;
        u16 baseLo;
        u8 baseMid;
        u8 flags;
        u8 grand;
        u8 baseHi;
    } __attribute__((packed));

    struct GDTPointer {
        u16 size;
        u32 base;
    } __attribute__((packed));

    Descriptor descriptors[MAX_DESCRIPTORS];

    void setDescriptor(u32 i, u64 base, u64 limit, u8 access, u8 grand);
};

#endif