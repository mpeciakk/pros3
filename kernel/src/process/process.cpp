
#include <process/process.hpp>
#include <memory/vmm.hpp>

extern "C" void* bootPageDirectory;

ProcessManager::ProcessManager() : currentPID(0) {
    instance = this;
}

Process* ProcessManager::createKernelProcess() {
    Process* process = new Process();

    process->name = "Kernel";
    process->id = currentPID++;
    process->pageDirPhys = virt2phys(&bootPageDirectory);

    processes.pushBack(process);

    return process;
}
