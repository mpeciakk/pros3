#include <panic.hpp>
#include <terminal.hpp>

void panic(char* message, char* file, int line, TrapFrame* frame) {
    Terminal::instance->write("--------------------bruh moment--------------------\n");
    Terminal::instance->write("kernel just commited bruh moment\n");
    Terminal::instance->write("why? cause ");
    Terminal::instance->write(message);
    Terminal::instance->write("\n");

    if (file) {
        Terminal::instance->write("where? in ");
        Terminal::instance->write(file);
        Terminal::instance->write("\n");
    }

    while (true) {}
}