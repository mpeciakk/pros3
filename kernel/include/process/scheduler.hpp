#ifndef __SCHEDULER_HPP
#define __SCHEDULER_HPP

#include <hardware/idt.hpp>
#include <lib/vector.hpp>
#include <process/thread.hpp>

#define SCHEDULER_FREQUENCY 30

class ProcessScheduler : public InterruptHandler {
public:
    ProcessScheduler();

    void addThread(Thread* thread);

private:
    Vector<Thread*> threads;

    Thread* currentThread = nullptr;
    int currentThreadNumber = -1;

    u32 frequency;
    u32 tickCount;

    u32 handle(u32 esp) override;

    Thread* getNextReadyThread(CPUState* state);
};

#endif
