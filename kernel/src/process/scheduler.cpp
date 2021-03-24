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

u32 ProcessScheduler::handle(u32 esp) {
    printk("0x20 interrupt fired\n");

    if (threads.size() > 0) {
        currentThread = getNextReadyThread();

        // TODO: jump into usermode if needed

        esp = (u32) currentThread->regs;

        if (currentThread->parent && currentThread->parent->pageDirPhys != 0) {
            VirtualMemoryManager::instance->switchPageDirectory(currentThread->parent->pageDirPhys);
        }

        setTSS(0x10, (u32) currentThread->stack + THREAD_STACK_SIZE);
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
