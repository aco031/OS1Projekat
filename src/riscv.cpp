//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../lib/console.h"

#define READ_SAVED_REG(offset, variable) \
    __asm__ volatile("ld %0, " #offset "(s0)" : "=r"(variable))

#define WRITE_SAVED_A0(value)                         \
    do                                                \
    {                                                 \
        uint64 _ret = (uint64)(value);                \
        __asm__ volatile("sd %0, 80(s0)" : : "r"(_ret)); \
    } while (false)


void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 syscallCode;
        READ_SAVED_REG(80, syscallCode);   // saved a0

        switch (syscallCode)
        {
            case 0x11:
            {
                // thread_create(thread_t* handle, void (*body)(void*), void* arg)

                uint64 rawHandle;
                uint64 rawBody;
                uint64 rawArg;

                READ_SAVED_REG(88, rawHandle);   // saved a1
                READ_SAVED_REG(96, rawBody);     // saved a2
                READ_SAVED_REG(104, rawArg);     // saved a3

                TCB** handle = (TCB**) rawHandle;
                TCB::Body body = (TCB::Body) rawBody;
                void* arg = (void*) rawArg;

                if (handle == nullptr || body == nullptr)
                {
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                // Privremeno rešenje bez pravog mem_alloc:
                // stack se alocira preko postojećeg operatora new[].
                uint64* stack = new uint64[DEFAULT_STACK_SIZE];

                if (stack == nullptr)
                {
                    *handle = nullptr;
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                *handle = TCB::createThread(body, arg, stack);

                if (*handle != nullptr)
                {
                    WRITE_SAVED_A0(0);
                }
                else
                {
                    delete[] stack;
                    WRITE_SAVED_A0((uint64)-1);
                }

                break;
            }

            case 0x12:
            {
                // thread_exit()

                WRITE_SAVED_A0(0);

                TCB::running->setFinished(true);
                TCB::timeSliceCounter = 0;
                TCB::dispatch();

                break;
            }

            case 0x13:
            {
                // thread_dispatch()

                TCB::timeSliceCounter = 0;
                TCB::dispatch();

                break;
            }

            default:
            {
                // Za sada ignorišemo ostale syscall kodove.
                break;
            }
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // supervisor software interrupt / timer
        mc_sip(SIP_SSIP);

        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();

            TCB::timeSliceCounter = 0;
            TCB::dispatch();

            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        // external interrupt / console
        console_handler();
    }
    else
    {
        // unexpected trap cause
    }
}

/*void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        mc_sip(SIP_SSIP);
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else
    {
        // unexpected trap cause
    }
}*/