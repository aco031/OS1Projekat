//
// Created by os on 6/12/26.
//

#ifndef SEM_HPP
#define SEM_HPP

#include "list.hpp"
#include "tcb.hpp"
#include "scheduler.hpp"

class _sem
{
public:
    explicit _sem(unsigned init);

    int wait();
    int signal();
    int close();

    int waitN(unsigned n);
    int signalN(unsigned n);

    bool isClosed() const { return closed; }

private:
    void unblockReadyThreads();

    unsigned value;
    bool closed;
    List<TCB> blockedQueue;
};

#endif // SEM_HPP