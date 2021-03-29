#include <Lib/Log.hpp>
#include <Lib/Panic.hpp>

void panic(char* message, TrapFrame* frame) {
    printf("--------------------bruh moment--------------------\n");
    printf("Kernel just committed bruh moment\n");
    printf("why? cause %s\n", message);

//    if (frame != nullptr) {
//        printk("eax = 0x%x\n, ebx = 0x%x\n, ecx = 0x%x\n, edx = 0x%x\n", frame->eax, frame->ebx, frame->ecx, frame->edx);
//        printk("ebp = 0x%x\n, edi = 0x%x\n, esi = 0x%x\n", frame->ebp, frame->edi, frame->esi);
//        printk("flags = 0x%x\n, cs = 0x%x\n, ss = 0x%x\n", frame->flags, frame->cs, frame->ss);
//        printk("eip = 0x%x\n, esp = 0x%x\n", frame->eip, frame->esp);
//        printk("errorcode = 0x%x\n", frame->error);
//    }

    while (true) {
    }
}