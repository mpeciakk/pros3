#include <panic.hpp>
#include <terminal.hpp>

void panic(char* message, char* file, int line, TrapFrame* frame) {
    Terminal::instance->write("--------------------KERNEL PANIC--------------------\n");
    Terminal::instance->write("Message = ");
    Terminal::instance->write(message);
    Terminal::instance->write("\n");

    if (file) {
        Terminal::instance->write("File = ");
        Terminal::instance->write(file);
        Terminal::instance->write("\n");
    }
}