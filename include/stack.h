/*******************************************************************************
 * File             : stack.h
 * Directory        : pvm/include
 * Author           : Muhammad Adriano Raksi
 * Created          : 02-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the interface of the VM's stack. Unlike other data segments in the
 * VM, each threads possess their own stack.
 ******************************************************************************/

#ifndef STACK_H
#define STACK_H 5

#include "common.h"

/*
 * Address type to be used only within the stack. @TODO: might want to
 * universalise the internal VM address implementation
 */
typedef va_t stackva_t;

typedef struct PineVMStack
{
    /* Stack pointer holds the index in the stack array that is free. */
    stackva_t pointer;

    /* Where all the data that are pushed into the stack is stored. */
    PrimitiveData *primdata_arr;
} Stack;

/*
 * Function : stk_Push
 * -------------------
 * Inserts PrimitiveData input into stack. Increments stack pointer.
 *
 * @param   : Pointer to Stack instance
 * @param   : PrimitiveData instance
 * @return  : Updated stack pointer
 */
stackva_t stk_Push(Stack *, PrimitiveData);

/*
 * Function : stk_Pop
 * -------------------
 * Frees PrimitiveData that the stack pointer points to.
 *
 * @param   : Pointer to Stack instance
 * @return  : Updated stack pointer
 */
stackva_t stk_Pop(Stack *);

/*
 * Function : stk_Peek
 * -------------------
 * Returns a copy of PrimitiveData that the stack pointer points to.
 *
 * @param   : Stack instance
 * @return  : PrimitiveData instance
 */
PrimitiveData      stk_Peek(Stack); /* Return TOS */

#endif /* STACK_H */
