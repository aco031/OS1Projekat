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
    if (myHandle != nullptr && myHandle->isFinished())
    {
        delete myHandle;
        myHandle = nullptr;
    }
}

char Console::getc()
{
    return ::getc();
}

void Console::putc(char c)
{
    ::putc(c);
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
    //return time_sleep(time);
    return -1;
}

void Thread::runWrapper(void* thread)
{
    if (thread != nullptr)
    {
        ((Thread*) thread)->run();
    }
}

Semaphore::Semaphore(unsigned init)
    : myHandle(nullptr)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    if (myHandle != nullptr)
    {
        sem_close(myHandle);
        myHandle = nullptr;
    }
}

int Semaphore::wait()
{
    if (myHandle == nullptr)
    {
        return -1;
    }

    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    if (myHandle == nullptr)
    {
        return -1;
    }

    return sem_signal(myHandle);
}

int Semaphore::waitN(unsigned n)
{
    if (myHandle == nullptr)
    {
        return -1;
    }

    return sem_wait_n(myHandle, n);
}

int Semaphore::signalN(unsigned n)
{
    if (myHandle == nullptr)
    {
        return -1;
    }

    return sem_signal_n(myHandle, n);
}