/*#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

extern void userMain();

static volatile bool userMainFinished = false;

static void userMainWrapper(void*)
{
    userMain();
    userMainFinished = true;
}

int main()
{
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB* mainThread = TCB::createThread(nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    thread_t userThread;
    thread_create(&userThread, userMainWrapper, nullptr);

    while (!userMainFinished)
    {
        thread_dispatch();
    }

    return 0;
}*/

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

extern void userMain();

static volatile bool userMainFinished = false;

static void userMainWrapper(void*)
{
    userMain();
    userMainFinished = true;
}

int main()
{
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB* mainThread = TCB::createThread(nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    thread_t userThread;
    thread_create(&userThread, userMainWrapper, nullptr);

    while (!userMainFinished)
    {
        thread_dispatch();
    }

    return 0;
}