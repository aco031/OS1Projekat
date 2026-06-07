//
// Created by os on 6/7/26.
//

#ifndef OS1PROJEKAT_CCB_HPP
#define OS1PROJEKAT_CCB_HPP


class ccb {
public:
    using Body = void (*)();

    static int createThread(thread_t *handle, Body body, void* arg, uint64* stack);

    static void yield();

    static TCB *running;

    static TCB* createThread1(thread_t *handle, Body body, void* arg, uint64* stack);

    static int exit();

    void setStatus(Status s);

    static void dispatch();

    Status getStatus();

    static int threadSleep(time_t time);


private:

    Body body;
    uint64 *stack;
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };


    void* arg;
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);



    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
    Status status;


};


#endif //OS1PROJEKAT_CCB_HPP
