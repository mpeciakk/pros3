#ifndef __SCHEDULER_HPP
#define __SCHEDULER_HPP

#include <Hardware/IDT.hpp>
#include <Lib/Vector.hpp>
#include <Tasking/Thread.hpp>

#define SCHEDULER_FREQUENCY 30

class ProcessScheduler : public InterruptHandler {
public:
    ProcessScheduler();
    ~ProcessScheduler() = default;

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
