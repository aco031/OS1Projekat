//
// Created by marko on 20.4.22..
//

#include "../h/memoryAllocator.hpp"

using size_t = decltype(sizeof(0));

void* operator new(size_t n)
{
    return MemoryAllocator::mem_allocBytes(n);
}

void* operator new[](size_t n)
{
    return MemoryAllocator::mem_allocBytes(n);
}

void operator delete(void* p) noexcept
{
    MemoryAllocator::mem_free(p);
}

void operator delete[](void* p) noexcept
{
    MemoryAllocator::mem_free(p);
}

void operator delete(void* p, size_t) noexcept
{
    MemoryAllocator::mem_free(p);
}

void operator delete[](void* p, size_t) noexcept
{
    MemoryAllocator::mem_free(p);
}
