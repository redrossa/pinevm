/*******************************************************************************
 * File             : codeseg.h
 * Directory        : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 23-04-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's code segment. The code segments stores the
 * input bytecode of the user program when the VM runs.
 ******************************************************************************/

#ifndef CODESEG_H
#define CODESEG_H 6

#include "common.h"

typedef struct PineVMCodeSegment
{
    /*
     * The number of bytes the bytecode file has is stored here. Calculated
     * during the opening of the file when initialising code segment. Used to
     * limit the array that contains the each bytecode.
     */
    size_t size;

    /*
     * This is where the bytecode is stored. The raw bytecode file is read per
     * byte when the VM is initialised. Running thread points directly to the
     * codes stored in here, no further copy is necessary.
     */
    opcode_t *content;

    /*
     * The bytecode file path that was passed by the user when running the this
     * VM program on the console.
     */
    const char *filepath;
} CodeSeg;

/*
 * Function : csg_Initialise
 * ------------------------
 * Initialises code segment. The bytecode File path is stored in 'filepath' and
 * opens the file. Calculates the file size and chops them into bytes and store
 * them into 'content'
 *
 * @param   : Pointer to CodeSeg instance
 * @param   : Bytecode file path
 * @return  : Error code
 */
int csg_Initialise(CodeSeg *, const char *);

/*
 * Function : csg_Finalise
 * ------------------------
 * Finalises code segment by freeing 'content'.
 *
 * @param   : Pointer to CodeSeg instance
 * @return  : Error code
 */
int csg_Finalise(CodeSeg *);

#endif /* CODESEG_H */
