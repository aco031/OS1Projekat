//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, void* arg, uint64* stackSpace)
{
    if (body != nullptr && stackSpace == nullptr) { return nullptr; }
    return new TCB(body, nullptr, arg, stackSpace, DEFAULT_TIME_SLICE);
}

TCB *TCB::createThread(BodyNoArg body)
{
    uint64* stack = body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr;
    if (body != nullptr && stack == nullptr) { return nullptr; }

    return new TCB(nullptr, body, nullptr, stack, DEFAULT_TIME_SLICE);
}

TCB *TCB::createThread(decltype(nullptr))
{
    return new TCB(nullptr, nullptr, nullptr, nullptr, DEFAULT_TIME_SLICE);
}

void TCB::yield()
{
    thread_dispatch();
}

void TCB::dispatch()
{
    TCB *old = running;

    if (old != nullptr && !old->isFinished() && !old->isBlocked())
    {
        Scheduler::put(old);
    }

    TCB *next = Scheduler::get();

    if (next == nullptr)
    {
        if (old != nullptr && !old->isFinished() && !old->isBlocked())
        {
            running = old;
            return;
        }

        // Deadlock situacija: nema spremne niti, a stara nit ne sme da nastavi
        // jer je finished ili blocked. Za sada nemaš idle nit, pa ovde stajemo.
        while (true) {}
    }

    running = next;

    if (old != nullptr && old != running)
    {
        TCB::contextSwitch(&old->context, &running->context);
    }
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();

    if (running->body != nullptr)
    {
        running->body(running->arg);
    }
    else if (running->bodyNoArg != nullptr)
    {
        running->bodyNoArg();
    }

    thread_exit();

    // Ne bi trebalo nikad da se dođe ovde.
    while (true)
    {
        thread_dispatch();
    }
}
