//
// Created by os on 6/11/26.
//

#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "syscall_c.hpp"

void* operator new(size_t size);
void* operator new[](size_t size);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

// GCC nekad generiše i ove "sized delete" pozive.
void operator delete(void* ptr, size_t size) noexcept;
void operator delete[](void* ptr, size_t size) noexcept;

class Thread
{
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    static void dispatch();

    // Za sada samo stub, jer time_sleep još ne radiš.
    static int sleep(time_t time);

protected:
    Thread();

    virtual void run() {}

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void runWrapper(void* thread);
};

class Semaphore
{
public:
    Semaphore(unsigned init = 1);

    virtual ~Semaphore();

    int wait();

    int signal();

	int waitN(unsigned n);

	int signalN(unsigned n);

private:
    sem_t myHandle;
};

class Console
{
public:
    static char getc();
    static void putc(char);
};

#endif