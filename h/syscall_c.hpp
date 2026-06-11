//
// Created by os on 6/11/26.
//

#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP

#include "../lib/hw.h"

class TCB;
typedef TCB* thread_t;

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

#endif //SYSCALL_C_HPP
