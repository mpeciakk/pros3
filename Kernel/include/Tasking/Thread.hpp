#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <Lib/Singleton.hpp>
#include <Tasking/Process.hpp>

#define THREAD_STACK_SIZE 4 * 1024

#define SEG_USER_DATA 0x23
#define SEG_USER_CODE 0x1B
#define SEG_KERNEL_DATA 0x10
#define SEG_KERNEL_CODE 8

enum ThreadState { Blocked, Ready, Stopped, Started };

class Process;

struct CPUState {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;

    u32 esi; // stack index
    u32 edi; // data index
    u32 ebp; // base pointer

    u32 error;

    u32 eip; // instruction pointer
    u32 cs;  // code segment
    u32 flags;
    u32 esp; // stack pointer
    u32 ss;  // stack segment
} __attribute__((packed));

struct Thread {
    Process* parent;
    CPUState* registers;
    u8* stack;
    u8* userStack;
    u32 userStackSize;
    ThreadState state;
};

class ThreadManager : public Singleton<ThreadManager> {
public:
    ThreadManager();
    ~ThreadManager() = default;

    Thread* createFromFunction(void (*entryPoint)(), bool isKernel = false, u32 flags = 0x202, Process* parent = 0);
};

#endif
