ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS {
    . = 0xC0100000;

    kernelStart = .;

    .text ALIGN (4K) : AT(ADDR(.text)-0xC0000000) {
        *(.multiboot)
        *(.text)
    }
    .rodata ALIGN (4K) : AT(ADDR(.rodata)-0xC0000000) {
        *(.rodata*)
    }

    .data ALIGN (4K) : AT(ADDR(.data)-0xC0000000) {
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));

        *(.data)
    }

    .bss ALIGN (4K) : AT(ADDR(.bss)-0xC0000000) {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }

    kernelEnd = .;

    kernelSize = kernelEnd - kernelStart;
}

ASSERT(kernelSize < 0x300000, "Kernel size exceeds 3MB");