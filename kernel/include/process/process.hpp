#ifndef __PROCESS_HPP
#define __PROCESS_HPP

#include <lib/vector.hpp>
#include <lib/singleton.hpp>
#include <process/thread.hpp>

class Thread;

struct Process {
    int id;
    bool isUserspace;
    Vector<Thread*> threads;
    char* name;
    u32 pageDirPhys;
};

class ProcessManager : public Singleton<ProcessManager> {
public:
    Vector<Process*> processes;

    ProcessManager();

    Process* createKernelProcess();

private:
    u32 currentPID;
};

#endif
