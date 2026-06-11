#include "../h/syscall_c.hpp"

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg)
{
    register uint64 a0 asm("a0") = 0x11;
    register uint64 a1 asm("a1") = (uint64) handle;
    register uint64 a2 asm("a2") = (uint64) start_routine;
    register uint64 a3 asm("a3") = (uint64) arg;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2), "r"(a3)
        : "memory"
    );

    return (int) a0;
}

int thread_exit()
{
    register uint64 a0 asm("a0") = 0x12;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        :
        : "memory"
    );

    return (int) a0;
}

void thread_dispatch()
{
    register uint64 a0 asm("a0") = 0x13;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        :
        : "memory"
    );
}