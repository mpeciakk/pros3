#include <memory/heap.hpp>
#include <process/thread.hpp>

ThreadManager::ThreadManager() {
    instance = this;
}

Thread* ThreadManager::createFromFunction(void (*entryPoint)(), bool isKernel, u32 flags, Process* parent) {
    Thread* thread = new Thread();

    thread->parent = parent;
    thread->stack = (u8*) MemoryManager::instance->alignedMalloc(THREAD_STACK_SIZE);
    thread->userStack = parent == nullptr ? (u8*) USER_STACK : (u8*) (USER_STACK - USER_STACK_SIZE * parent->threads.size());
    thread->userStackSize = USER_STACK_SIZE;
    thread->regs = (CPUState*) ((u32) thread->stack + THREAD_STACK_SIZE - sizeof(CPUState));
    thread->regs->eax = 0;
    thread->regs->ebx = 0;
    thread->regs->ecx = 0;
    thread->regs->edx = 0;
    thread->regs->esi = 0;
    thread->regs->edi = 0;
    thread->regs->ebp = 0;
    thread->regs->eip = (u32) entryPoint;
    thread->regs->cs = SEG_KERNEL_CODE;
    thread->regs->flags = flags;
    thread->regs->esp = (u32) thread->stack + THREAD_STACK_SIZE;

    return thread;
}
