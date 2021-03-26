#include <hardware/gdt.hpp>
#include <lib/log.hpp>
#include <memory/heap.hpp>
#include <process/scheduler.hpp>

ProcessScheduler::ProcessScheduler() : InterruptHandler(0x20) {
    instance = this;
}

void ProcessScheduler::addThread(Thread* thread) {
    threads.pushBack(thread);
}

void dumpFrame(CPUState* frame) {
    printk("EAX = 0x%x\n", frame->eax);
    printk("EBX = 0x%x\n", frame->ebx);
    printk("ECX = 0x%x\n", frame->ecx);
    printk("EDX = 0x%x\n", frame->edx);
    printk("EBP = 0x%x\n", frame->ebp);
    printk("EDI = 0x%x\n", frame->edi);
    printk("ESI = 0x%x\n", frame->esi);
    printk("EIP = 0x%x\n", frame->eip);
    printk("ESP = 0x%x\n", frame->esp);

    printk("CS = 0x%x\n", frame->cs);
    printk("ES = 0x%x\n", frame->es);
    printk("DS = 0x%x\n", frame->ds);
    printk("GS = 0x%x\n", frame->gs);
    printk("FS = 0x%x\n", frame->fs);
}

u32 ProcessScheduler::handle(u32 esp) {
    printk("0x20 interrupt fired\n");

    if (threads.size() > 0) {
        currentThread = getNextReadyThread();

        // TODO: jump into usermode if needed

        esp = (u32) currentThread->regs;

//        if (currentThread->parent && currentThread->parent->pageDirPhys != 0) {
//            VirtualMemoryManager::instance->switchPageDirectory(currentThread->parent->pageDirPhys);
//        }
//
//        setTSS(0x10, (u32) currentThread->stack + THREAD_STACK_SIZE);
    }

    return esp;
}

Thread* ProcessScheduler::getNextReadyThread() {
    Thread* thread = threads[currentThreadNumber];

    if (currentThreadNumber++ > threads.size() - 1) {
        currentThreadNumber = 0;
    }

    return thread;
}
