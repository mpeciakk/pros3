#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <process/process.hpp>

#define THREAD_STACK_SIZE 4 * 1024

#define SEG_USER_DATA 0x23
#define SEG_USER_CODE 0x1B
#define SEG_KERNEL_DATA 0x10
#define SEG_KERNEL_CODE 8

class Process;

struct CPUState {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
    u32 esi;
    u32 edi;
    u32 ebp;
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;

    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} __attribute__((packed));

struct Thread {
    Process* parent;
    u8* stack;
    u8* userStack;
    u32 userStackSize;
    CPUState* regs;
};

class ThreadManager : Singleton<ThreadManager> {
public:
    ThreadManager();

    Thread* createFromFunction(void (*entryPoint)(), bool isKernel = false, u32 flags = 0x202, Process* parent = 0);
};

#endif
