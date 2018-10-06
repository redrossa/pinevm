/*******************************************************************************
 * File             : heap.h
 * Directory        : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 26-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's heap. The heap is implemented as a
 * superset of the static segment, so the size must be explicitly stated prior
 * to the running of this program. Similarly, the heap is implemented in a way
 * such that there is an outer array (the actual heap), which holds an inner
 * array (a complex data) made up of VM's primitive data. This segment has a
 * minimum space of 1, reserved for null address.
 ******************************************************************************/

#ifndef HEAP_H
#define HEAP_H 8

#include "common.h"
#include <stdbool.h>

typedef struct PineVMHeapFrame
{
    /* If this frame is free or not */
    bool    free;

    /* Size of frame, which is actually the PrimitiveData array 'block' */
    size_t  framesize;

    /* Heap block */
    PrimitiveData *block;
} HeapFrame;

typedef struct PineVMHeap
{
     /* Total free frames */
    size_t      freeframes;

     /* Total blocks allocated */
    size_t      totalblocks;

    /* Total size of pool */
    size_t      size;

     /* The actual heap */
    HeapFrame *var_pool;
} Heap;

/*
 * Function : heap_Initialise
 * --------------------------
 * Initialises heap. Allocates pool.
 *
 * @param   : Pointer to Heap instance
 * @param   : Size of pool to allocate
 * @return  : Error code
 */
int heap_Initialise(Heap *, size_t size);

/*
 * Function : heap_Finalise
 * ------------------------
 * Finalises Heap. Frees unfreed blocks and finally the whole pool.
 *
 * @param   : Pointer to Heap instance
 * @return  : Error code
 */
int heap_Finalise(Heap *);

/*
 * Function : heap_Malloc
 * ----------------------
 * Allocate a frame in the pool with a given amount blocks.
 *
 * @param   : Pointer to Heap instance
 * @param   : Address in the pool
 * @param   : Size of frame
 * @return  : Error code
 */
int heap_Malloc(Heap *, va_t, size_t);

/*
 * Function : heap_Calloc
 * ----------------------
 * Allocate a frame in the pool with a given amount of blocks then clear the
 * blocks.
 *
 * @param   : Pointer to Heap instance
 * @param   : Address in the pool
 * @param   : Size of frame
 * @return  : Error code
 */
int heap_Calloc(Heap *, va_t, size_t);

/*
 * Function : heap_Realloc
 * -----------------------
 * Enlarges/shrinks a frame, retaining its current data if possible.
 *
 * @param   : Pointer to Heap instance
 * @param   : Address in the pool
 * @param   : Size of frame
 * @return  : Error code
 */
int heap_Realloc(Heap *, va_t, size_t);

/*
 * Function : heap_Free
 * --------------------
 * Frees a frame, allowing it to be reused again later.
 *
 * @param   : Pointer to Heap instance
 * @param   : Address in the pool (of the frame)
 * @return  : Error code
 */
int heap_Free(Heap *, va_t);

#endif /* HEAP_H */
