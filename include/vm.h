/*******************************************************************************
 * File             : vm.h
 * Path             : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 08-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM. Don't call any VM functions other than the
 * ones written in this file as all VM configuration functions and other
 * instances for users are provided here.
 ******************************************************************************/

#ifndef VM_H
#define VM_H 0

#include "memmap.h"
#include "codeseg.h"
#include "staticseg.h"
#include "heap.h"
#include "core.h"

typedef struct PineVM
{
    /*
     * Maps the memory layout of the VM. Stores the addresses of the first
     * frames of each segments.
     */
    MemMap memmap;

    /*
     * The input bytecode file is stored here. An array of int of size 1 stores
     * all the codes. Additional file information may be stored here too. @see:
     * pvm/include/codeseg.h to see the actual interface of the VM's code
     * segment.
     */
    CodeSeg codeseg;

    /*
     * @TODO: Update implementation
     * Stores static data of the program. Allocation for this segment within the
     * VM must be made explicit in the bytecode.
     */
    StaticSeg staticseg;

    /*
     * One of the main data storage. Like static segment, allocation for this
     * segment within the VM must be made explicit in the bytecode. The
     * implementation of the heap is a superset of the static segment. @see:
     * pvm/include/heap.h to see the actual interface of the heap.
     */
    Heap heap;

    /*
     * All the VM operations that the user wants to see are performed here.
     * Previous struct members are only complimentary parts of the VM's total
     * data storage. The core includes a program clock (scheduler), threads and
     * fast memory storages such as registers and stack. @see: pvm/include/
     * core.h to see the core's interface.
     */
    Core core;
} VM;

/*
 * Function : pvm_initialise
 * -------------------------
 * Creates a VM instance and initialises its members.
 *
 * @param   : Bytecode file path
 * @return  : Initialised VM
 *
 */
VM pvm_initialise(const char *);

/*
 * Function : pvm_finalise
 * -----------------------
 * Finalise a VM instance's members, free any dynamically allocated objects.
 * Reusing a VM instance that has been passed to this function results in an
 * undefined behaviour.
 *
 * @param   : Pointer to VM instance
 * @return  : Error code
 */
int pvm_finalise(VM *);

/*
 * Function : pvm_run
 * ------------------
 * Read bytecode stored in the code segment and perform operations. Once all
 * operations from the bytecodes are performed, call pvm_finalise! Running a VM
 * twice results in an undefined behaviour

 * @param   : Pointer to VM instance
 * @return  : Error code
 */
int pvm_run(VM *);

/*
 * Function : pvm_handle
 * ---------------------
 * Configures PineVMHandler.
 *
 * @TODO    : The error handler seems very inconsistent in this project. Please
 *            review the current error handler. Any updates on the error handler
 *            could make this function obsolete.
 * @param   : Header number
 * @param   : Caller function name
 * @param   : Return code from the function
 * @return  : Calls pvm_Dispatch
 * @deprecated
 */
int pvm_handle(int, const char *, int);

/*
 * Function : pvm_dispatch
 * -----------------------
 * Handles error codes passed by the VM's internal functions. Determines if the
 * program has to force exit or note.
 *
 * @TODO    : The error handler seems very inconsistent in this project. Please
 *            review the current error handler. Any updates on the error handler
 *            could make this function obsolete.
 * @param   : Error code
 * @return  : Error code
 * @deprecated
 */
int pvm_dispatch(int);

int pvm_reporterror(int, const char *, const char *);

#endif /* VM_H */
