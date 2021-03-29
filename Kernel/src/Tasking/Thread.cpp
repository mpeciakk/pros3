#include <Tasking/Thread.hpp>

ThreadManager::ThreadManager() {
    instance = this;
}

Thread* ThreadManager::createFromFunction(void (*entryPoint)(), bool isKernel, u32 flags, Process* parent) {
    auto* thread = new Thread();

    thread->stack = new u8[THREAD_STACK_SIZE];

    thread->registers = (CPUState*) (thread->stack + THREAD_STACK_SIZE - sizeof(CPUState));

    thread->registers->eax = 0;
    thread->registers->ebx = 0;
    thread->registers->ecx = 0;
    thread->registers->edx = 0;

    thread->registers->esi = 0;
    thread->registers->edi = 0;
    thread->registers->ebp = 0;

    thread->registers->eip = (u32)(entryPoint);
    thread->registers->cs = 0x08;
    thread->registers->flags = 0x202;

    thread->state = ThreadState::Ready;

    return thread;
}
