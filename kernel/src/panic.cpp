#include <panic.hpp>
#include <lib/log.hpp>

void panic(char* message, char* file, int line, TrapFrame* frame) {
    printf("--------------------bruh moment--------------------\n");
    printf("kernel just committed bruh moment\n");
    printf("why? cause %s\n", message);

    if (file) {
        printf("where? in ");
        printf(file);
        printf("\n");
    }

    while (true) {}
}