/*******************************************************************************
 * File             : core.h
 * Path             : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 07-08-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's core. The core is where all the user
 * bytecode instructions are performed.
 ******************************************************************************/

#ifndef CORE_H
#define CORE_H 1

#include "opcode.h"

#define THREAD_LIMIT 256

typedef struct PineVMCore
{
    /* An index that points to the running thread in thread_pool */
    uint8_t running_thread;

    /* Number of threads spawned */
    size_t thread_num;

    /*
     * The thread pool size is precompiled. This means that there is a maximum
     * number of threads spawned possible and also takes up the same amount of
     * space throughout the lifetime of this program. Although the threads are
     * already created, they are virtually not alive because this program needs
     * to explicitly spawn a thread its own way to be used by this program.
     */
    Thread thread_pool[THREAD_LIMIT];

    /* Scheduler syncs all the threads work */
    Scheduler scheduler;
} Core;

/*
 * Function : core_initialise
 * -------------------------
 * Initialises Core (its members).
 *
 * @param   : Pointer to VM instance
 * @return  : Error code
 */
int core_initialise(VM *);

/*
 * Function : core_finalise
 * -------------------------
 * Finalises Core, free any dynamically allocated members.
 *
 * @param   : Pointer to VM instance
 * @return  : Error code
 */
int core_finalise(VM *);

/*
 * Function : core_run
 * -------------------------
 * Runs core, spawns the master thread and run it.
 *
 * @param   : Pointer to VM instance
 * @return  : Error code
 */
int core_run(VM *);

/*
 * Function : core_managethread
 * -------------------------
 * Finds the thread that needs to be run after the previous thread has finished
 * a cycle. Could possibly set previous thread to sleep.
 *
 * @NOTE    : Called only by thread
 * @param   : Pointer to VM instance
 * @param   : Thread ID of the previous running thread
 * @return  : Error code
 */
int core_managethread(VM *, va_t);

/*
 * Function : core_cycle
 * -------------------------
 * Keep track of any sleeping threads' cooldown. Cycles scheduler.
 *
 * @NOTE    : Called only by thread.
 * @param   : Pointer to VM instance
 * @param   : Tthread ID of the previous running thread
 * @return  : Error code
 */
int core_cycle(VM *, va_t);

#endif /* CORE_H */
