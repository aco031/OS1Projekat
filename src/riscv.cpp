//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/sem.hpp"
#include "../lib/console.h"
#include "../h/memoryAllocator.hpp"

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
	// Nove korisnicke niti treba da krenu u korisnickom rezimu
    // SPP = 0 znaci da ce sret vratiti procesor u U-mode
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);

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
			case 0x01:
            {
                // mem_alloc(size_t sizeInBlocks)

                uint64 rawSize;
                READ_SAVED_REG(88, rawSize); // saved a1

                void* ret = MemoryAllocator::mem_alloc((size_t) rawSize);

                WRITE_SAVED_A0((uint64) ret);
                break;
            }

            case 0x02:
            {
                // mem_free(void* ptr)

                uint64 rawPtr;
                READ_SAVED_REG(88, rawPtr); // saved a1

                int ret = MemoryAllocator::mem_free((void*) rawPtr);

                WRITE_SAVED_A0(ret);
                break;
            }

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

            case 0x21:
            {
                // sem_open(sem_t* handle, unsigned init)

                uint64 rawHandle;
                uint64 rawInit;

                READ_SAVED_REG(88, rawHandle);   // saved a1
                READ_SAVED_REG(96, rawInit);     // saved a2

                _sem** handle = (_sem**) rawHandle;
                unsigned init = (unsigned) rawInit;

                if (handle == nullptr)
                {
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                *handle = new _sem(init);

                if (*handle != nullptr)
                {
                    WRITE_SAVED_A0(0);
                }
                else
                {
                    WRITE_SAVED_A0((uint64)-1);
                }

                break;
            }

            case 0x22:
            {
                // sem_close(sem_t handle)

                uint64 rawHandle;
                READ_SAVED_REG(88, rawHandle);   // saved a1

                _sem* handle = (_sem*) rawHandle;

                if (handle == nullptr)
                {
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                int ret = handle->close();

                if (ret == 0)
                {
                    delete handle;
                }

                WRITE_SAVED_A0(ret);
                break;
            }

            case 0x23:
            {
                // sem_wait(sem_t id)

                uint64 rawHandle;
                READ_SAVED_REG(88, rawHandle);   // saved a1

                _sem* handle = (_sem*) rawHandle;

                if (handle == nullptr)
                {
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                int ret = handle->wait();

                WRITE_SAVED_A0(ret);
                break;
            }

            case 0x24:
            {
                // sem_signal(sem_t id)

                uint64 rawHandle;
                READ_SAVED_REG(88, rawHandle);   // saved a1

                _sem* handle = (_sem*) rawHandle;

                if (handle == nullptr)
                {
                    WRITE_SAVED_A0((uint64)-1);
                    break;
                }

                int ret = handle->signal();

                WRITE_SAVED_A0(ret);
                break;
            }

			case 0x25:
			{
    			// sem_wait_n(sem_t id, unsigned n)

    			uint64 rawHandle;
    			uint64 rawN;

    			READ_SAVED_REG(88, rawHandle);   // saved a1
   				READ_SAVED_REG(96, rawN);        // saved a2

    			_sem* handle = (_sem*) rawHandle;
    			unsigned n = (unsigned) rawN;

    			if (handle == nullptr)
    			{
        			WRITE_SAVED_A0((uint64)-1);
        			break;
    			}

    			int ret = handle->waitN(n);

				WRITE_SAVED_A0(ret);
				break;
			}

			case 0x26:
			{
    			// sem_signal_n(sem_t id, unsigned n)

    			uint64 rawHandle;
    			uint64 rawN;

    			READ_SAVED_REG(88, rawHandle);   // saved a1
    			READ_SAVED_REG(96, rawN);        // saved a2

    			_sem* handle = (_sem*) rawHandle;
    			unsigned n = (unsigned) rawN;

    			if (handle == nullptr)
    			{
        			WRITE_SAVED_A0((uint64)-1);
        			break;
    			}

    			int ret = handle->signalN(n);

    			WRITE_SAVED_A0(ret);
    			break;
			}

			case 0x41:
            {
                // getc()

                char c = __getc();

                WRITE_SAVED_A0((uint64)c);
                break;
            }

            case 0x42:
            {
                // putc(char c)

                uint64 rawChar;
                READ_SAVED_REG(88, rawChar); // saved a1

                __putc((char)rawChar);

                WRITE_SAVED_A0(0);
                break;
            }

            default:
            {
                // Ignorisemo ostale syscall kodove
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
    else if (scause == 0x0000000000000002UL)
    {
        // Illegal instruction
        // Test 7 namerno pokusava da izvrsi privilegovanu instrukciju iz korisnickog rezima
        // Ako dodjemo ovde, to znaci da se korisnicki kod stvarno izvrsava u U-mode

        const char* msg =
            "\nIllegal instruction trap.\n";

        while (*msg != '\0')
        {
            __putc(*msg);
            msg++;
        }

        while (true) {}
    }
    else
    {
        const char* msg = "\nUnexpected trap.\n";

        while (*msg != '\0')
        {
            __putc(*msg);
            msg++;
        }

        while (true) {}
    }
}
