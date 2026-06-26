//
// Created by os on 6/14/26.
//

#include "../h/memoryAllocator.hpp"

MemoryAllocator::FreeMem* MemoryAllocator::freeMemHead = nullptr;

size_t MemoryAllocator::bytesToBlocks(size_t size)
{
    if (size == 0)
    {
        return 0;
    }

    return (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
}

void MemoryAllocator::init()
{
    uint64 start = (uint64) HEAP_START_ADDR;
    uint64 end = (uint64) HEAP_END_ADDR;

    // Poravnamo heap na granice blokova
    start = (start + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;
    end = end / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;

    freeMemHead = (FreeMem*) start;
    freeMemHead->next = nullptr;
    freeMemHead->size = (end - start) / MEM_BLOCK_SIZE;
}

void* MemoryAllocator::mem_allocBytes(size_t size)
{
    size_t blocks = bytesToBlocks(size);
    return mem_alloc(blocks);
}

void* MemoryAllocator::mem_alloc(size_t size)
{
    if (size == 0 || freeMemHead == nullptr)
    {
        return nullptr;
    }

    // Treba nam jedan dodatni blok za header
    size_t needed = size + 1;

    FreeMem* prev = nullptr;
    FreeMem* curr = freeMemHead;

    while (curr != nullptr)
    {
        if (curr->size >= needed)
        {
            // Ako ostaje dovoljno prostora za novi slobodan fragment
            if (curr->size > needed)
            {
                FreeMem* newFree = (FreeMem*) ((char*) curr + needed * MEM_BLOCK_SIZE);
                newFree->next = curr->next;
                newFree->size = curr->size - needed;

                if (prev != nullptr)
                {
                    prev->next = newFree;
                }
                else
                {
                    freeMemHead = newFree;
                }

                curr->size = needed;
            }
            else
            {
                // Uzimamo ceo fragment
                if (prev != nullptr)
                {
                    prev->next = curr->next;
                }
                else
                {
                    freeMemHead = curr->next;
                }
            }

            curr->next = nullptr;

            // Korisniku vracamo adresu posle jednog header bloka
            return (char*) curr + MEM_BLOCK_SIZE;
        }

        prev = curr;
        curr = curr->next;
    }

    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr)
{
    if (ptr == nullptr)
    {
        return -1;
    }

    if ((uint64) ptr < (uint64) HEAP_START_ADDR || (uint64) ptr >= (uint64) HEAP_END_ADDR)
    {
        return -2;
    }

    FreeMem* block = (FreeMem*) ((char*) ptr - MEM_BLOCK_SIZE);

    FreeMem* prev = nullptr;
    FreeMem* curr = freeMemHead;

    while (curr != nullptr && curr < block)
    {
        prev = curr;
        curr = curr->next;
    }

    block->next = curr;

    if (prev != nullptr)
    {
        prev->next = block;
    }
    else
    {
        freeMemHead = block;
    }

    // Spoji sa sledecim ako su susedni
    if (block->next != nullptr &&
        (char*) block + block->size * MEM_BLOCK_SIZE == (char*) block->next)
    {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    // Spoji sa prethodnim ako su susedni
    if (prev != nullptr &&
        (char*) prev + prev->size * MEM_BLOCK_SIZE == (char*) block)
    {
        prev->size += block->size;
        prev->next = block->next;
    }

    return 0;
}