/*******************************************************************************
 * File             : core.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 07-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's core. The core controls each thread_pool
 * in the user's program, such as keeping track of which thread_pool to run and
 * having all thread_pool synchronised by using scheduler.
 ******************************************************************************/

#include "../include/core.h"
#include "../include/vm.h"

int core_initialise(VM *vm)
{
    Core *tmp = &vm->core;

    tmp->thread_num = 0;
    *tmp->thread_pool = (Thread) {0};
    sch_initialise(&tmp->scheduler);

    return 0;
}

int core_finalise(VM *vm)
{
    Core *tmp = &vm->core;

    /* Traverse through thread pool to finalise all alive thread_pool */
    for (va_t i = 0x0; i < THREAD_LIMIT; i++)
        if ((tmp->thread_pool[i].flag & (THR_ALIVE | THR_RUN | THR_SLEEP)) == (THR_ALIVE | THR_RUN | THR_SLEEP))
            thr_kill(vm, i);

    return 0;
}

inline int core_run(VM *vm)
{
    thr_spawn(vm, 0x10); /* Spawn Master Thread */
    return thr_run(vm, 0x0); /* Run Master Thread */
}

int core_managethread(VM *vm, va_t tid)
{
    if (vm->core.thread_pool[0x0].flag & THR_DEAD)
        return 0;

    va_t i;
    Core *tmp = &vm->core;
    Thread *next = &vm->core.thread_pool[VA_SUCCEED(tid)];

    /* i = next thread */
    i = next->flag & THR_UNINIT ? 0x0 :
        next->flag & THR_DEAD ? VA_SUCCEED(VA_SUCCEED(tid)) :
        next->flag & THR_SLEEP && next->countdown > 0 ? VA_SUCCEED(tid) :
        tid;

    /* Sleep original thread if the the next thread to run is not the same */
    if (i != tid && tmp->thread_pool[tid].flag & THR_RUN)
        thr_sleep(vm, tid, tid-1);

    return (i == tid && tmp->thread_pool[tid].flag & THR_DEAD) ? core_finalise(vm):  thr_run(vm, i);
}

int core_cycle(VM *vm, va_t tid)
{
    Core *tmp = &vm->core;

    /* Cycle scheduler */
    sch_cycle(&tmp->scheduler);

    /* Decrement thread_pool countdown */
    for (va_t i = 0x0; tmp->thread_pool[i].flag != THR_UNINIT; i++)
        if (tmp->thread_pool[i].flag == THR_SLEEP)
            tmp->thread_pool[i].countdown--;

    return core_managethread(vm, tid);
}
