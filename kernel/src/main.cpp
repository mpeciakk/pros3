#include <multiboot.hpp>
#include <terminal.hpp>
#include <types.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, u32 multibootMagic) {
    Terminal terminal;
    terminal.clear();
    terminal.write("pros");

    while (true) {
    }
}
