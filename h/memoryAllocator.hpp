//
// Created by os on 6/14/26.
//

#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator
{
public:
    static void init();

    // Koristi kernel/ABI: size je broj korisnickih blokova
    static void* mem_alloc(size_t size);

    // Koristi operator new/new[]: size je broj bajtova
    static void* mem_allocBytes(size_t size);

    static int mem_free(void* ptr);

private:
    struct FreeMem
    {
        FreeMem* next;
        size_t size; // velicina slobodnog fragmenta u blokovima
    };

    static FreeMem* freeMemHead;

    static size_t bytesToBlocks(size_t size);
};

#endif // MEMORY_ALLOCATOR_HPP