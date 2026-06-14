//
// Created by os on 6/12/26.
//

#include "../h/sem.hpp"

_sem::_sem(unsigned init)
    : value(init), closed(false)
{
}

int _sem::wait()
{
    return waitN(1);
}

int _sem::signal()
{
    return signalN(1);
}

int _sem::waitN(unsigned n)
{
    if (closed || TCB::running == nullptr)
    {
        return -1;
    }

    if (n == 0)
    {
        return 0;
    }

    // FIFO ponašanje:
    // ako već neko čeka, nova nit ne preskače red,
    // čak i ako trenutno ima dovoljno resursa za nju.
    if (blockedQueue.peekFirst() == nullptr && value >= n)
    {
        value -= n;
        return 0;
    }

    TCB::running->setBlocked(true);
    TCB::running->setSemRetVal(0);
    TCB::running->setSemWaitUnits(n);

    blockedQueue.addLast(TCB::running);

    TCB::timeSliceCounter = 0;
    TCB::dispatch();

    return TCB::running->getSemRetVal();
}

int _sem::signalN(unsigned n)
{
    if (closed)
    {
        return -1;
    }

    if (n == 0)
    {
        return 0;
    }

    value += n;

    unblockReadyThreads();

    return 0;
}

void _sem::unblockReadyThreads()
{
    while (blockedQueue.peekFirst() != nullptr)
    {
        TCB* thread = blockedQueue.peekFirst();

        if (thread->getSemWaitUnits() > value)
        {
            break;
        }

        thread = blockedQueue.removeFirst();

        value -= thread->getSemWaitUnits();

        thread->setSemWaitUnits(0);
        thread->setBlocked(false);
        thread->setSemRetVal(0);

        Scheduler::put(thread);
    }
}

int _sem::close()
{
    if (closed)
    {
        return -1;
    }

    closed = true;

    while (blockedQueue.peekFirst() != nullptr)
    {
        TCB* thread = blockedQueue.removeFirst();

        thread->setSemWaitUnits(0);
        thread->setBlocked(false);
        thread->setSemRetVal(-1);

        Scheduler::put(thread);
    }

    return 0;
}

/*
#include "../h/sem.hpp"

_sem::_sem(unsigned init)
    : value((int)init), closed(false)
{
}

int _sem::wait()
{
    if (closed || TCB::running == nullptr)
    {
        return -1;
    }

    value--;

    if (value < 0)
    {
        TCB::running->setBlocked(true);
        TCB::running->setSemRetVal(0);

        blockedQueue.addLast(TCB::running);

        TCB::timeSliceCounter = 0;
        TCB::dispatch();

        return TCB::running->getSemRetVal();
    }

    return 0;
}

int _sem::signal()
{
    if (closed)
    {
        return -1;
    }

    value++;

    if (value <= 0)
    {
        TCB* thread = blockedQueue.removeFirst();

        if (thread != nullptr)
        {
            thread->setBlocked(false);
            thread->setSemRetVal(0);
            Scheduler::put(thread);
        }
    }

    return 0;
}

int _sem::close()
{
    if (closed)
    {
        return -1;
    }

    closed = true;

    while (blockedQueue.peekFirst() != nullptr)
    {
        TCB* thread = blockedQueue.removeFirst();

        thread->setBlocked(false);
        thread->setSemRetVal(-1);
        Scheduler::put(thread);
    }

    return 0;
}*/