//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    bool isBlocked() const { return blocked; }

    void setFinished(bool value) { finished = value; }

    void setBlocked(bool value) { blocked = value; }

    void setSemRetVal(int value) { semRetVal = value; }

	void setSemWaitUnits(unsigned value) { semWaitUnits = value; }

	unsigned getSemWaitUnits() const { return semWaitUnits; }

    uint64 getTimeSlice() const { return timeSlice; }

    int getSemRetVal() const { return semRetVal; }

    using Body = void (*)(void*);
    using BodyNoArg = void (*)();

    static TCB *createThread(Body body, void* arg, uint64* stackSpace);

    // Samo zbog kompatibilnosti sa starim testovima
    static TCB *createThread(BodyNoArg body);

    // Samo zbog TCB::createThread(nullptr) za main nit
    static TCB *createThread(decltype(nullptr));

    static void yield();

    static TCB *running;

private:
    TCB(Body body, BodyNoArg bodyNoArg, void* arg, uint64* stackSpace, uint64 timeSlice) :
            body(body),
            bodyNoArg(bodyNoArg),
            arg(arg),
            stack(stackSpace),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            finished(false),
            blocked(false),
            semRetVal(0),
			semWaitUnits(0)
    {
        if (body != nullptr || bodyNoArg != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    BodyNoArg bodyNoArg;
    void* arg;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool blocked;
    int semRetVal;
	unsigned semWaitUnits;

    friend class Riscv;
    friend class _sem;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
