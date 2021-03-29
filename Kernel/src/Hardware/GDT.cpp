#include <Hardware/GDT.hpp>
#include <Lib/String.hpp>

static GDT::TSSEntry tss{};

GDT::GDT() {
    setDescriptor(0, 0, 0, 0, 0);
    setDescriptor(1, 0, 0xffffffff, 0x9A, 0xCF);
    setDescriptor(2, 0, 0xffffffff, 0x92, 0xCF);
    setDescriptor(3, 0, 0xffffffff, 0xFA, 0xCF);
    setDescriptor(4, 0, 0xffffffff, 0xF2, 0xCF);
    installTSS(0x10, 0x00);

    GDTPointer pointer{};
    pointer.size = (MAX_DESCRIPTORS * sizeof(Descriptor)) - 1;
    pointer.base = (u32) &descriptors[0];
    asm volatile("lgdt %0" : : "m"(pointer));

    asm("movw %0, %%ax;"
        "movw %%ax, %%ds;"
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;"
        "ljmp %1, $next;"
        "next:;" ::"i"(0x10),
        "i"(0x08));
    asm("movw %0, %%ax;"
        "movw %%ax, %%ss;"
        :
        : "i"(0x10));

    asm("ltr %0" : : "r"((u16) 0x2B));
}

void GDT::setDescriptor(u32 i, u64 base, u64 limit, u8 access, u8 grand) {
    memset((void*) &descriptors[i], 0, sizeof(Descriptor));

    descriptors[i].baseLo = base & 0xffff;
    descriptors[i].baseMid = (base >> 16) & 0xff;
    descriptors[i].baseHi = (base >> 24) & 0xff;
    descriptors[i].limit = limit & 0xffff;

    descriptors[i].flags = access;
    descriptors[i].grand = (limit >> 16) & 0x0f;
    descriptors[i].grand |= grand & 0xf0;
}

void GDT::installTSS(u16 kernelSS, u16 kernelESP) {
    u32 base = (u32) &tss;

    setDescriptor(5, base, base + sizeof(TSSEntry), 0xE9, 0);

    memset((void*) &tss, 0, sizeof(TSSEntry));
    tss.ss0 = kernelSS;
    tss.esp0 = kernelESP;

    tss.cs = 0x0B;
    tss.ss = 0x13;
    tss.es = 0x13;
    tss.ds = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
    tss.iomap = sizeof(TSSEntry);
}

void setTSS(u16 kernelSS, u16 kernelESP) {
    tss.ss0 = kernelSS;
    tss.esp0 = kernelESP;
}