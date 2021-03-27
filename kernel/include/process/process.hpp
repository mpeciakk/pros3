#ifndef __PROCESS_HPP
#define __PROCESS_HPP

#include <lib/vector.hpp>
#include <process/thread.hpp>
#include <types.hpp>

#define PROC_USER_HEAP_SIZE 1 * 1024 * 1024

class Thread;

struct Process {
    int id;
    bool isUserspace;
    Vector<Thread*> threads;
    char* name;
    u32 pageDirPhys;

    struct Executable {
        u32 memoryBase;
        u32 memorySize;
    } executable;

    struct Heap {
        u32 heapStart;
        u32 heapEnd;
    } heap;
};

class ProcessManager {
public:
    Vector<Process*> processes;

    ProcessManager();

    Process* createKernelProcess();
    Process* createProcess(char* fileName, bool isKernel);

private:
    u32 currentPID = 0;
};

#endif
