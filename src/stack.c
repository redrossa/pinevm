/*******************************************************************************
 * File             : stack.c
 * Path             : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 03-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of the VM's stack. Stack is implemented as a LIFO
 * based primdata_array.
 ******************************************************************************/

#include "../include/stack.h"

#define STACK_SIZE 0x400

int stk_initialise(Stack *stack)
{
    *stack = (Stack) {.pointer = 0, .primdata_arr = malloc(sizeof(PrimitiveData) *STACK_SIZE)};
    return stack->primdata_arr == NULL ? pvm_reporterror(STACK_H, __FUNCTION__, "Allocation failed") : 0;
}

int stk_finalise(Stack *stack)
{
    free(stack->primdata_arr);
    return 0;
}

stackva_t stk_push(Stack *stack, PrimitiveData data)
{
    if (stack->pointer == STACK_SIZE)
        pvm_reporterror(STACK_H, __FUNCTION__, "Stack overflow");
    stack->primdata_arr[stack->pointer] = data;
    return ++stack->pointer;
}

stackva_t stk_pop(Stack *stack)
{
    if (stack->pointer == 0)
        pvm_reporterror(STACK_H, __FUNCTION__, "Stack underflow");
    return --stack->pointer;
}

PrimitiveData stk_peek(Stack stack)
{
    if (stack.pointer == STACK_SIZE)
        pvm_reporterror(STACK_H, __FUNCTION__, "Stack underflow");
    return stack.primdata_arr[stack.pointer - 1];
}
