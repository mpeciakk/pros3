#include <hardware/gdt.hpp>
#include <lib/log.hpp>
#include <memory/vmm.hpp>
#include <process/scheduler.hpp>

ProcessScheduler::ProcessScheduler() : InterruptHandler(0x20) {
    tickCount = 0;
    frequency = SCHEDULER_FREQUENCY;
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
}

u32 ProcessScheduler::handle(u32 esp) {
    tickCount++;

    if (tickCount == frequency) {
        tickCount = 0;

        if (threads.size() > 0) {
            Thread* thread = getNextReadyThread((CPUState*) esp);

            while (thread->state == Stopped) {
                log(0, "Removing thread %x from system", (u32) thread);
//                threads.remove(thread);
                delete thread;

                thread = getNextReadyThread((CPUState*) esp);
            }

            if (currentThread != nullptr && currentThread->state == Stopped) {
                log(0, "Removing thread %x from system", (u32) currentThread);
//                threads.remove(currentThread);
                delete currentThread;
            }

            printk("%d\n", (u32) thread->registers->eip);

            esp = (u32) thread->registers;

            if (thread->parent && thread->parent->pageDirPhys != 0) {
                VirtualMemoryManager::instance->switchPageDirectory(thread->parent->pageDirPhys);
            }

            setTSS(0x10, (u32) thread->stack + THREAD_STACK_SIZE);
        }
    }

    return esp;
}

Thread* ProcessScheduler::getNextReadyThread(CPUState* state) {
    printk("t\n");

    if (currentThreadNumber >= 0) {
        threads[currentThreadNumber]->registers = state;
    }

    if (++currentThreadNumber >= threads.size()) {
        currentThreadNumber %= threads.size();
    }

    return threads[currentThreadNumber];
}