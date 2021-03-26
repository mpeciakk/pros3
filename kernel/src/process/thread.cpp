#include <memory/heap.hpp>
#include <process/thread.hpp>

ThreadManager::ThreadManager() {
    instance = this;
}

Thread* ThreadManager::createFromFunction(void (*entryPoint)(), bool isKernel, u32 flags, Process* parent) {
    Thread* thread = new Thread();

    thread->parent = parent;
    thread->stack = new u8[THREAD_STACK_SIZE];
    //    thread->userStack = parent == nullptr ? (u8*) USER_STACK : (u8*) (USER_STACK - USER_STACK_SIZE * parent->threads.size());
    //    thread->userStackSize = USER_STACK_SIZE;
    thread->regs = (CPUState*) ((u32) thread->stack + THREAD_STACK_SIZE - sizeof(CPUState));
    thread->regs->eax = 0;
    thread->regs->ebx = 0;
    thread->regs->ecx = 0;
    thread->regs->edx = 0;
    thread->regs->esi = 0;
    thread->regs->edi = 0;
    thread->regs->ebp = 0;
    thread->regs->esp = (u32) thread->stack + THREAD_STACK_SIZE;
    thread->regs->eip = (u32) entryPoint;
    thread->regs->cs = isKernel ? SEG_KERNEL_CODE : SEG_USER_CODE;
    thread->regs->ds = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;
    thread->regs->es = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;
    thread->regs->fs = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;
    thread->regs->gs = isKernel ? SEG_KERNEL_DATA : SEG_USER_DATA;
    thread->regs->eflags = flags;

    return thread;
}
