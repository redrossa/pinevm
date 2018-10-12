/*******************************************************************************
 * File             : thread.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 04-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's thread. Thread is the most basic unit
 * that performs operations that the user ordered.
 ******************************************************************************/

#include "../include/thread.h"
#include "../include/vm.h"

int thr_spawn(VM *vm, va_t instrpointreg)
{
    va_t i;
    Thread *tmp;

    /* Find an uninitialised thread */
    for (i = 0x0; vm->core.thread_pool[i].flag != THR_UNINIT; i++);

    /* Initialise thread */
    tmp = &vm->core.thread_pool[i];
    tmp->flag = THR_ALIVE;
    tmp->countdown = 0;
    tmp->controlunit.progcountreg = 0;
    tmp->controlunit.instrpointreg = instrpointreg;

    vm->core.thread_num++;

    return 0;
}

int thr_kill(VM *vm, va_t tid)
{
    Thread *tmp = &vm->core.thread_pool[tid];

    /* Check if thread is dead already */
    if (tmp->flag & THR_DEAD)
        return pvm_reporterror(THREAD_H, __FUNCTION__, NULL);

    if (tmp->flag & THR_RUN)
        tmp->flag = THR_DEAD;

    vm->core.thread_num--;

    return 0;
}

int thr_run(VM *vm, va_t tid)
{
    Thread *tmp = &vm->core.thread_pool[tid];

    if (tmp->flag & (THR_DEAD) || ((tmp->flag & THR_SLEEP) && tmp->countdown > 0))
        return core_managethread(vm, tid);

    vm->core.running_thread = tid;
    tmp->flag = THR_RUN;
    tmp->controlunit.progcountreg++;
    tmp->controlunit.instrreg = vm->codeseg.content[tmp->controlunit.instrpointreg++];

    opc_Execute[tmp->controlunit.instrreg](vm, tid); /* Actual VM operation */

    return core_cycle(vm, tid);
}

int thr_sleep(VM *vm, va_t tid, vmclock_t cycle)
{
    Thread *tmp = &vm->core.thread_pool[tid];
    tmp->flag = THR_SLEEP;
    tmp->countdown = cycle;

    return 0;
}
