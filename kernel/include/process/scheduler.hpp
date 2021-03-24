#ifndef __SCHEDULER_HPP
#define __SCHEDULER_HPP

#include <process/thread.hpp>
#include <hardware/idt.hpp>
#include <lib/singleton.hpp>

class ProcessScheduler : public Singleton<ProcessScheduler>, public InterruptHandler {
public:
    ProcessScheduler();

    void addThread(Thread* thread);

private:
    Vector<Thread*> threads;
    Thread* currentThread = nullptr;
    u32 currentThreadNumber = 0;

    u32 handle(u32 esp) override;

    Thread* getNextReadyThread();
};

#endif
