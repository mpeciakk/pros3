#include <multiboot.hpp>
#include <types.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, u32 multibootMagic) {
    u16* memory = (u16*) 0xC00B8000;

    for (u32 i = 0; i < 25; i++) {
        for (u32 j = 0; j < 80; j++) {
            memory[i * 80 + j] = ' ' | 1 << 0;
        }
    }

    memory[0] = 'p' | 1 << 8;
    memory[1] = 'r' | 2 << 8;
    memory[2] = 'o' | 3 << 8;
    memory[3] = 's' | 4 << 8;

    while (true) {
    }
}
