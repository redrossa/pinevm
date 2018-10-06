/*******************************************************************************
 * File             : vm.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 26-04-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM. The VM is merely a way to encapsulate
 * all the other units of this project.
 ******************************************************************************/

#include "../include/vm.h"

static struct
{
    VM *vm;
    int unit; /* VM Internal Unit of caller function */
    const char *caller; /* Name of caller function */
    int retcode; /* Return code of caller function */
} PineVMHandler;

VM pvm_Initialise(const char *path)
{
    /* Create VM Handler */
    VM vm;
    PineVMHandler.vm = &vm;

    /* Initialise data areas */
    csg_Initialise(&vm.codeseg, path);
    ssg_Initialise(&vm.staticseg, (size_t) vm.codeseg.content[0] << 56 | (size_t) vm.codeseg.content[1] << 48 | (size_t) vm.codeseg.content[2] << 40 | (size_t) vm.codeseg.content[3] << 32 | vm.codeseg.content[4] << 24 | vm.codeseg.content[5] << 16 | vm.codeseg.content[6] << 8 | vm.codeseg.content[7]);
    heap_Initialise(&vm.heap, (size_t) vm.codeseg.content[8] << 56 | (size_t) vm.codeseg.content[9] << 48 | (size_t) vm.codeseg.content[10] << 40 | (size_t) vm.codeseg.content[11] << 32 | vm.codeseg.content[12] << 24 | vm.codeseg.content[13] << 16 | vm.codeseg.content[14] << 8 | vm.codeseg.content[15]);
    core_Initialise(&vm);

    return vm;
}

int pvm_Finalise(VM *vm)
{
    csg_Finalise(&vm->codeseg);
    ssg_Finalise(&vm->staticseg);
    heap_Finalise(&vm->heap);
    return 0;
}

inline int pvm_Run(VM *vm)
{
    return core_Run(vm);
}

int pvm_Handle(int unit, const char *caller, int retcode)
{
    PineVMHandler.unit = unit;
    PineVMHandler.caller = caller;
    PineVMHandler.retcode = retcode;
    return pvm_Dispatch(retcode);
}

int pvm_Dispatch(int retcode)
{
    if (retcode == 1)
    {
        /* Force finalise VM to exit */
        fprintf(stderr, "%d: %s: %d\n", PineVMHandler.unit, PineVMHandler.caller, PineVMHandler.retcode);
        pvm_Finalise(PineVMHandler.vm);
    }
    return retcode;
}

inline int pvm_ReportError(int vmunit, const char *caller, const char *msg)
{
    if (msg == NULL)
        fprintf(stderr, "Error at %d, %s\n", vmunit, caller);
    else
        fprintf(stderr, "Error at %d, %s: %s\n", vmunit, caller, msg);
    exit(1);
}
