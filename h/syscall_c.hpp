//
// Created by os on 6/11/26.
//

#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP

#include "../lib/hw.h"

class TCB;
typedef TCB* thread_t;

class _sem;
typedef _sem* sem_t;
//
void* mem_alloc(size_t size);
int mem_free(void* ptr);

int time_sleep(time_t time);

char getc();
void putc(char c);
//
int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int sem_wait_n(sem_t id, unsigned n);

int sem_signal_n(sem_t id, unsigned n);

#endif //SYSCALL_C_HPP
