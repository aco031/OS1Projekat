//
// Created by os on 6/11/26.
//

#include "../h/syscall_cpp.hpp"
#include "../h/tcb.hpp"

Thread::Thread(void (*body)(void*), void* arg)
    : myHandle(nullptr), body(body), arg(arg)
{
}

Thread::Thread()
    : myHandle(nullptr), body(&Thread::runWrapper), arg(this)
{
}

Thread::~Thread()
{
    if (myHandle == nullptr)
    {
        return;
    }

    // Minimalno "join" ponašanje za test:
    // delete Thread objekta sačeka da se njegova nit stvarno završi.
    while (!myHandle->isFinished())
    {
        thread_dispatch();
    }

    delete myHandle;
    myHandle = nullptr;
}

int Thread::start()
{
    if (myHandle != nullptr)
    {
        return -1;
    }

    if (body == nullptr)
    {
        return -1;
    }

    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch()
{
    thread_dispatch();
}

int Thread::sleep(time_t time)
{
    (void) time;

    // Još nemaš time_sleep sistemski poziv.
    return -1;
}

void Thread::runWrapper(void* thread)
{
    if (thread != nullptr)
    {
        ((Thread*) thread)->run();
    }
}