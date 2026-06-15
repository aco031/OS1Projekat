#include "../h/syscall_c.hpp"
#include "../h/memoryAllocator.hpp"
#include "../lib/console.h"

void* mem_alloc(size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }

    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    register uint64 a0 asm("a0") = 0x01;
    register uint64 a1 asm("a1") = (uint64) blocks;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (void*) a0;
}

int mem_free(void* ptr)
{
    register uint64 a0 asm("a0") = 0x02;
    register uint64 a1 asm("a1") = (uint64) ptr;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (int) a0;
}

int time_sleep(time_t)
{
    return -1;
}

char getc()
{
    register uint64 a0 asm("a0") = 0x41;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        :
        : "memory"
    );

    return (char) a0;
}

void putc(char c)
{
    register uint64 a0 asm("a0") = 0x42;
    register uint64 a1 asm("a1") = (uint64) c;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );
}

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

int sem_open(sem_t* handle, unsigned init)
{
    register uint64 a0 asm("a0") = 0x21;
    register uint64 a1 asm("a1") = (uint64) handle;
    register uint64 a2 asm("a2") = (uint64) init;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2)
        : "memory"
    );

    return (int) a0;
}

int sem_close(sem_t handle)
{
    register uint64 a0 asm("a0") = 0x22;
    register uint64 a1 asm("a1") = (uint64) handle;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (int) a0;
}

int sem_wait(sem_t id)
{
    register uint64 a0 asm("a0") = 0x23;
    register uint64 a1 asm("a1") = (uint64) id;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (int) a0;
}

int sem_signal(sem_t id)
{
    register uint64 a0 asm("a0") = 0x24;
    register uint64 a1 asm("a1") = (uint64) id;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1)
        : "memory"
    );

    return (int) a0;
}

int sem_wait_n(sem_t id, unsigned n)
{
    register uint64 a0 asm("a0") = 0x25;
    register uint64 a1 asm("a1") = (uint64) id;
    register uint64 a2 asm("a2") = (uint64) n;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2)
        : "memory"
    );

    return (int) a0;
}

int sem_signal_n(sem_t id, unsigned n)
{
    register uint64 a0 asm("a0") = 0x26;
    register uint64 a1 asm("a1") = (uint64) id;
    register uint64 a2 asm("a2") = (uint64) n;

    __asm__ volatile(
        "ecall"
        : "+r"(a0)
        : "r"(a1), "r"(a2)
        : "memory"
    );

    return (int) a0;
}