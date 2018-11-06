/*******************************************************************************
 * File             : vm.c
 * Path             : pvm/src
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

VM pvm_initialise(const char *path)
{
    /* Create VM Handler */
    VM vm;
    PineVMHandler.vm = &vm;
    FILE * fp;
    uint32_t fheader;
    uint64_t staticsize;

    /* Open file for reading */
    fp = fopen(path, "rb");
    if (fp == NULL)
        pvm_reporterror(CODESEG_H, __FUNCTION__, "File not found");

    /* Check file header */
    fread(&fheader, sizeof(uint32_t), 1, fp);
    fheader = REVERSE_32(fheader);
    if (fheader != MAGIC_NUMBER)
        pvm_reporterror(VM_H, __FUNCTION__, "Incorrect file type");


    ssg_initialise(&vm.staticseg, fp);
    csg_initialise(&vm.codeseg, path, fp);
    heap_initialise(&vm.heap, 0x10000000);
    core_initialise(&vm);

    fclose(fp);

    return vm;
}

int pvm_finalise(VM *vm)
{
    ssg_finalise(&vm->staticseg);
    csg_finalise(&vm->codeseg);
    heap_finalise(&vm->heap);
    return 0;
}

inline int pvm_run(VM *vm)
{
    return core_run(vm);
}

inline int pvm_reporterror(int vmunit, const char *caller, const char *msg)
{
    if (msg == NULL)
        fprintf(stderr, "Error at %d, %s\n", vmunit, caller);
    else
        fprintf(stderr, "Error at %d, %s: %s\n", vmunit, caller, msg);
    exit(1);
}
