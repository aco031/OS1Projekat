//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../test/Threads_CPP_API_test.hpp"

int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB *mainThread = TCB::createThread(nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("Pokrecem TEST 2 - Threads CPP API\n");

    Threads_CPP_API_test();

    printString("TEST 2 finished\n");

    delete mainThread;

    return 0;
}

/*
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../test/Threads_C_API_test.hpp"

int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB *mainThread = TCB::createThread(nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("Pokrecem TEST 1 - Threads C API\n");

    Threads_C_API_test();

    printString("TEST 1 finished\n");

    delete mainThread;

    return 0;
}*/

/*
#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

int main()
{
    // Mora pre prvog ecall-a, jer thread_create sada ide preko ecall/syscall mehanizma.
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    TCB *mainThread = TCB::createThread(nullptr);
    TCB::running = mainThread;

    thread_t threads[4];

    int ret = 0;

    ret = thread_create(&threads[0], workerBodyA, nullptr);
    if (ret < 0) { printString("ThreadA create failed\n"); return ret; }
    printString("ThreadA created\n");

    ret = thread_create(&threads[1], workerBodyB, nullptr);
    if (ret < 0) { printString("ThreadB create failed\n"); return ret; }
    printString("ThreadB created\n");

    ret = thread_create(&threads[2], workerBodyC, nullptr);
    if (ret < 0) { printString("ThreadC create failed\n"); return ret; }
    printString("ThreadC created\n");

    ret = thread_create(&threads[3], workerBodyD, nullptr);
    if (ret < 0) { printString("ThreadD create failed\n"); return ret; }
    printString("ThreadD created\n");

    // Ovo može da ostane ako ti je timer već radio u Markovom kodu.
    // Za samo sinhroni dispatch nije presudno, ali ne smeta.
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(threads[0]->isFinished() &&
             threads[1]->isFinished() &&
             threads[2]->isFinished() &&
             threads[3]->isFinished()))
    {
        thread_dispatch();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }

    delete mainThread;

    printString("Finished\n");

    return 0;
}
*/

/*
#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"

int main()
{
    TCB *threads[5];

    threads[0] = TCB::createThread(nullptr);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(workerBodyA);
    printString("ThreadA created\n");
    threads[2] = TCB::createThread(workerBodyB);
    printString("ThreadB created\n");
    threads[3] = TCB::createThread(workerBodyC);
    printString("ThreadC created\n");
    threads[4] = TCB::createThread(workerBodyD);
    printString("ThreadD created\n");

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(threads[1]->isFinished() &&
             threads[2]->isFinished() &&
             threads[3]->isFinished() &&
             threads[4]->isFinished()))
    {
        TCB::yield();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }
    printString("Finished\n");

    return 0;
}*/
