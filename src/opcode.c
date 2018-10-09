/*******************************************************************************
 * File             : opcode.c
 * Directory        : pvm/src
 * Author           : Muhammad Adriano Raksi
 * Created          : 24-03-18 (DD-MM-YY)
 *------------------------------------------------------------------------------
 * Contains the implementation of all the available VM operations provided for
 * the user. Users only have access to call the functions of type opcode_t that
 * are listed here by using bytecodes.
 ******************************************************************************/

#include "../include/opcode.h"
#include "../include/vm.h"
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <sys/syscall.h>

PrimitiveData *fetch_reg(VM *, va_t);
uint8_t fetch_code(VM *, va_t);

/*
 * OPCODE FUNCTION PROTOTYPES
 */

opcode_t NOP(VM *, va_t);
opcode_t HLT(VM *, va_t);

/* REGISTER MANIPULATION */
opcode_t LOAD(VM *, va_t);
opcode_t MOVE(VM *, va_t);
opcode_t CAST(VM *, va_t);
/* END LOAD */

/* STACK INSTRUCTIONS */
opcode_t PUSH(VM *, va_t);
opcode_t POP(VM *, va_t);
opcode_t PUT(VM *, va_t);
opcode_t PEEK(VM *, va_t);
/* END STUCK INSTRUCTIONS */

/* HEAP INSTRUCTIONS */
opcode_t MALLOC(VM *, va_t);
opcode_t CALLOC(VM *, va_t);
opcode_t REALLOC(VM *, va_t);
opcode_t FREE(VM *, va_t);
opcode_t STORE(VM *, va_t);
opcode_t GET(VM *, va_t);
/* END HEAP INSTRUCTIONS */

/* STATIC SEGMENT INSTRUCTIONS */
opcode_t ALLOC_STATIC(VM *, va_t);
opcode_t STORE_STATIC(VM *, va_t);
opcode_t GET_STATIC(VM *, va_t);
/* END STATIC SEGMENT INSTRUCTIONS */

/* FLOW INSTRUCTIONS */
opcode_t JUMP(VM *, va_t);
opcode_t JUMP_IF_TRUE(VM *, va_t);
opcode_t JUMP_IF_FALSE(VM *, va_t);
opcode_t RETURN(VM *, va_t);
/* END FLOW INSTRUCTIONS */

/* ARITHMETIC & BITWISE OPERATIONS */
opcode_t ADD(VM *, va_t);
opcode_t SUB(VM *, va_t);
opcode_t MUL(VM *, va_t);
opcode_t DIV(VM *, va_t);
opcode_t MOD(VM *, va_t);
opcode_t AND(VM *, va_t);
opcode_t XOR(VM *, va_t);
opcode_t OR(VM *, va_t);
opcode_t NOT(VM *, va_t);
opcode_t LSHIFT(VM *, va_t);
opcode_t RSHIFT(VM *, va_t);
/* END ARITHMETIC & BITWISE OPERATIONS */

/* RELATIONAL & LOGICAL OPERATIONS */
opcode_t LESS(VM *, va_t);
opcode_t LESS_EQ(VM *, va_t);
opcode_t GREAT(VM *, va_t);
opcode_t GREAT_EQ(VM *, va_t);
opcode_t EQUAL(VM *, va_t);
opcode_t N_EQUAL(VM *, va_t);
opcode_t LOG_AND(VM *, va_t);
opcode_t LOG_OR(VM *, va_t);
opcode_t LOG_NOT(VM *, va_t);
/* END RELATIONAL & LOGICAL OPERATIONS */

/*
 * END OPCODE FUNCTION PROTOTYPES
 */

InstructionSet opc_Execute[256] =
{
    /* 0x00 */  NOP,

    /* 0x01 */  HLT,

    /* 0x02 */  LOAD, MOVE, CAST,

    /* 0x05 */  PUSH, POP, PUT, PEEK,

    /* 0x09 */  MALLOC, CALLOC, REALLOC, FREE, STORE, GET,

    /* 0x0E */  ALLOC_STATIC, STORE_STATIC, GET_STATIC,

    /* 0x12 */  JUMP, JUMP_IF_TRUE, JUMP_IF_FALSE,

    /* 0x15 */  ADD, SUB, MUL, DIV, MOD, AND, XOR, OR, NOT, LSHIFT, RSHIFT,

    /* 0x20 */  LESS, LESS_EQ, GREAT, GREAT_EQ, EQUAL, N_EQUAL, LOG_AND, LOG_OR, LOG_NOT
};

#define fetch_code_2BYTES(vm, tid) ((fetch_code(vm, tid) << 8) | fetch_code(vm, tid))

#define fetch_code_4BYTES(vm, tid) ((fetch_code_2BYTES(vm, tid) << 16) | fetch_code_2BYTES(vm, tid))

#define fetch_code_8BYTES(vm, tid) (((uint64_t) fetch_code_4BYTES(vm, tid) << 32) | fetch_code_4BYTES(vm, tid))

opcode_t NOP(VM *vm, va_t tid)
{
    return vm->core.thread_pool[tid].controlunit.instrreg;
}

opcode_t HLT(VM *vm, va_t tid)
{
    thr_Kill(vm, tid);
    return vm->core.thread_pool[tid].controlunit.instrreg;
}

#define DATA_RETRIEVER(data)\
(\
    (data).storage == I8 ? (data).i8 :\
    ((data).storage == I16 ? (data).i16 :\
    ((data).storage == I32 ? (data).i32 :\
    ((data).storage == I64 ? (data).i64 :\
    ((data).storage == UI8 ? (data).ui8 :\
    ((data).storage == UI16 ? (data).ui16 :\
    ((data).storage == UI32 ? (data).ui32 :\
    ((data).storage == UI64 ? (data).ui64 :\
    ((data).storage == DBL ? (data).dbl :\
    ((data).va)))))))))\
)

/*
 * REGISTER MANIPULATION
 */

opcode_t LOAD(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS} {2:TYPE} {3:RAW_DATA} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg, prot;
    opcode_t type;

    /* Fetch REGISTER_ADDRESS */
    reg = fetch_reg(vm, tid);

    /* Fetch TYPE */
    type = fetch_code(vm, tid);

    /* Fetch RAW_DATA */
    switch (type)
    {
        case 0:
            prot.storage = I8;
            prot.i8 = fetch_code(vm, tid);
            break;
        case 1:
            prot.storage = UI8;
            prot.ui8 = fetch_code(vm, tid);
            break;
        case 2:
            prot.storage = I16;
            prot.i16 = fetch_code_2BYTES(vm, tid);
            break;
        case 3:
            prot.storage = UI16;
            prot.ui16 = fetch_code_2BYTES(vm, tid);
            break;
        case 4:
            prot.storage = I32;
            prot.i32 = fetch_code_4BYTES(vm, tid);
            break;
        case 5:
            prot.storage = UI32;
            prot.ui32 = fetch_code_4BYTES(vm, tid);
            break;
        case 6:
            prot.storage = I64;
            prot.i64 = fetch_code_8BYTES(vm, tid);
            break;
        case 7:
            prot.storage = UI64;
            prot.ui64 = fetch_code_8BYTES(vm, tid);
            break;
        case 8:
            prot.storage = DBL;
            prot.dbl = fetch_code_8BYTES(vm, tid);
            break;
        case 9:
            prot.storage = VA;
            prot.va = fetch_code_4BYTES(vm, tid);
            break;
    }

    *reg = prot;

    return thread->controlunit.instrreg;
}

opcode_t MOVE(VM *vm, va_t tid) /* {0:OPCODE} {1:SOURCE_ADDRESS} {2:DESTINATION_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *src, *dest, prot;

    /* Fetch source register address */
    src = fetch_reg(vm, tid);

    prot = *src;

    /* Fetch destination register address */
    dest = fetch_reg(vm, tid);

    /* Destination register holds data in source register */
    *dest = prot;

    return thread->controlunit.instrreg;
}

opcode_t CAST(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS} {2:TYPE} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg, new;
    opcode_t type;

    /* Fetch REGISTER_ADDRESS */
    reg = fetch_reg(vm, tid);

    /* Fetch TYPE */
    type = fetch_code(vm, tid);

    switch (type)
    {
        case 0:
            new.storage = I8;
            new.i8 = DATA_RETRIEVER(*reg);
            break;
        case 1:
            new.storage = UI8;
            new.ui8 = DATA_RETRIEVER(*reg);
            break;
        case 2:
            new.storage = I16;
            new.i16 = DATA_RETRIEVER(*reg);
            break;
        case 3:
            new.storage = UI16;
            new.ui16 = DATA_RETRIEVER(*reg);
            break;
        case 4:
            new.storage = I32;
            new.i32 = DATA_RETRIEVER(*reg);
            break;
        case 5:
            new.storage = UI32;
            new.ui32 = DATA_RETRIEVER(*reg);
            break;
        case 6:
            new.storage = I64;
            new.i64 = DATA_RETRIEVER(*reg);
            break;
        case 7:
            new.storage = UI64;
            new.ui64 = DATA_RETRIEVER(*reg);
            break;
        case 8:
            new.storage = DBL;
            new.dbl = DATA_RETRIEVER(*reg);
            break;
        case 9:
            new.storage = VA;
            new.va = DATA_RETRIEVER(*reg);
            break;
    }

    *reg = new;

    return thread->controlunit.instrreg;
}

/* END LOAD */

/*
 *STACK INSTRUCTIONS
 */

opcode_t PUSH(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *data;

    data = fetch_reg(vm, tid); /* fetch DATA to be pushed to stack */
    stk_Push(&thread->stack, *data);

    return thread->controlunit.instrreg;
}

opcode_t POP(VM *vm, va_t tid) /* {0:OPCODE} */
{
    Thread *thread = &vm->core.thread_pool[tid];

    stk_Pop(&thread->stack);

    return thread->controlunit.instrreg;
}

opcode_t PUT(VM *vm, va_t tid) /* {0:OPCODE} {1:STACK_ADDRESS} {2:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t stack_va;
    PrimitiveData *reg;

    /* Fetch STACK_ADDRESS (8 bytes) */
    stack_va = fetch_code_8BYTES(vm, tid);

    /* Fetch register */
    reg = fetch_reg(vm, tid);

    /* Put data in register into stack of the given address */
    thread->stack.primdata_arr[stack_va] = *reg;

    return thread->controlunit.instrreg;
}

opcode_t PEEK(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg;

    reg = fetch_reg(vm, tid);
    *reg = stk_Peek(thread->stack);

    return thread->controlunit.instrreg;
}
/* END STACK INSTRUCTIONS */

/*
 *HEAP INSTRUCTIONS
 */

opcode_t MALLOC(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} {2:SIZE} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va;
    size_t size;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Fetch SIZE (8 bytes) */
    size = fetch_code_8BYTES(vm, tid);

    /* Malloc VM heap at address heap_va  */
    heap_Malloc(&vm->heap, heap_va, size);

    return thread->controlunit.instrreg;
}

opcode_t CALLOC(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} {2:SIZE} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va;
    size_t size;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Fetch SIZE (8 bytes) */
    size = fetch_code_8BYTES(vm, tid);

    /* Calloc VM heap at address heap_va  */
    heap_Calloc(&vm->heap, heap_va, size);

    return thread->controlunit.instrreg;
}

opcode_t REALLOC(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} {2:SIZE} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va;
    size_t size;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Fetch SIZE (8 bytes) */
    size = fetch_code_8BYTES(vm, tid);

    /* Malloc VM heap at address heap_va */
    heap_Realloc(&vm->heap, heap_va, size);

    return thread->controlunit.instrreg;
}

opcode_t FREE(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Free VM heap at address heap_va */
    heap_Free(&vm->heap, heap_va);

    return thread->controlunit.instrreg;
}

opcode_t STORE(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} {2:OFFSET_ADDRESS} {3:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va, offset;
    PrimitiveData *reg;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Fetch OFFSET_ADDRESS (8 bytes) */
    offset = fetch_code_8BYTES(vm, tid);

    /* Fetch register */
    reg = fetch_reg(vm, tid);

    /* Store data in given register to the address at heap */
    vm->heap.var_pool[heap_va].block[offset] = *reg;

    return thread->controlunit.instrreg;
}

opcode_t GET(VM *vm, va_t tid) /* {0:OPCODE} {1:HEAP_ADDRESS} {2:OFFSET_ADDRESS} {3:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t heap_va, offset;
    PrimitiveData *reg, prot;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    heap_va = fetch_code_8BYTES(vm, tid);

    /* Fetch OFFSET_ADDRESS (8 bytes) */
    offset = fetch_code_8BYTES(vm, tid);

    prot = vm->heap.var_pool[heap_va].block[offset];

    /* Fetch register */
    reg = fetch_reg(vm, tid);

    /* Get data in heap of given address to register */
    *reg = prot;

    return thread->controlunit.instrreg;
}

/* END HEAP INSTRUCTIONS */

/*
 *staticIC SEGMENT INSTRUCTIONS
 */

opcode_t ALLOC_STATIC(VM *vm, va_t tid) /* {0:OPCODE} {1:STATIC_ADDRESS} {2:SIZE} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t va;
    size_t size;

    /* Fetch staticIC_ADDRESS (8 bytes) */
    va = fetch_code_8BYTES(vm, tid);

    /* Fetch SIZE (8 bytes) */
    size = fetch_code_8BYTES(vm, tid);

    /* Allocate in staticic segment */
    ssg_Allocate(&vm->staticseg, va, size);

    return thread->controlunit.instrreg;
}

opcode_t STORE_STATIC(VM *vm, va_t tid) /* {0:OPCODE} {1:STATIC_ADDRESS} {2:OFFSET_ADDRESS} {3:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t va, offset;
    PrimitiveData *reg;

    /* Fetch HEAP_ADDRESS (8 bytes) */
    va = fetch_code_8BYTES(vm, tid);

    /* Fetch OFFSET_ADDRESS (8 bytes) */
    offset = fetch_code_8BYTES(vm, tid);

    /* Fetch register */
    reg = fetch_reg(vm, tid);

    /* Store data in given register to the address at staticic segment */
    vm->staticseg.var_pool[va].primdata_arr[offset] = *reg;

    return thread->controlunit.instrreg;
}

opcode_t GET_STATIC(VM *vm, va_t tid)  /* {0:OPCODE} {1:STATIC_ADDRESS} {2:OFFSET_ADDRESS} {3:REGISTER_ADDRESS} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t va, offset;
    PrimitiveData *reg, prot;

    /* Fetch STATIC_ADDRESS (8 bytes) */
    va = fetch_code_8BYTES(vm, tid);

    /* Fetch OFFSET_ADDRESS (8 bytes) */
    offset = fetch_code_8BYTES(vm, tid);

    prot = vm->staticseg.var_pool[va].primdata_arr[offset];

    /* Fetch register */
    reg = fetch_reg(vm, tid);

    /* Get static data in static segment of given address to register */
    *reg = prot;

    return thread->controlunit.instrreg;
}

/* END staticIC SEGMENT INSTRUCTIONS */

/*
 *FLOW INSTRUCTIONS
 */

opcode_t JUMP(VM *vm, va_t tid) /* {0:OPCODE} {1:CODESEG_INDEX} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t index_address;

    /* Fetch codeseg_INDEX to which program will jump to */
    index_address = fetch_code_8BYTES(vm, tid);
    /* Configure thread, jump to codeseg_INDEX */
    thread->flag = THR_RUN;
    thread->controlunit.progcountreg++;
    thread->controlunit.instrreg = vm->codeseg.content[thread->controlunit.instrpointreg = index_address];

    /* End cycle early */
    return core_Cycle(vm, tid);
}

opcode_t JUMP_IF_TRUE(VM *vm, va_t tid) /* {0:OPCODE} {1:CODESEG_INDEX} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t index_address;

    /* Fetch CODESEG_INDEX to which program will jump to */
    index_address = fetch_code_8BYTES(vm, tid);

    if (DATA_RETRIEVER(thread->controlunit.aritreg) == 1)
    {
        /* Configure thread, jump to CODESEG_INDEX */
        thread->flag = THR_RUN;
        thread->controlunit.progcountreg++;
        thread->controlunit.instrreg = vm->codeseg.content[thread->controlunit.instrpointreg = index_address];
        return core_Cycle(vm, tid);
    }
    return thread->controlunit.instrreg;
}

opcode_t JUMP_IF_FALSE(VM *vm, va_t tid) /* {0:OPCODE} {1:CODESEG_INDEX} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    va_t index_address;

    /* Fetch codeseg_INDEX to which program will jump to */
    index_address = fetch_code_8BYTES(vm, tid);

    if (DATA_RETRIEVER(thread->controlunit.aritreg) == 0)
    {
        /* Configure thread, jump to codeseg_INDEX */
        thread->flag = THR_RUN;
        thread->controlunit.progcountreg++;
        thread->controlunit.instrreg = vm->codeseg.content[thread->controlunit.instrpointreg = index_address];
        return core_Cycle(vm, tid);
    }
    return thread->controlunit.instrreg;
}

/* END FLOW INSTRUCTIONS */

/*
 *ARITHMETIC & BITWISE OPERATIONS
 */

 #define DATA_RETRIEVER_MATHS(data)\
 (\
     (data).storage == I8 ? (data).i8 :\
     ((data).storage == I16 ? (data).i16 :\
     ((data).storage == I32 ? (data).i32 :\
     ((data).storage == I64 ? (data).i64 :\
     ((data).storage == UI8 ? (data).ui8 :\
     ((data).storage == UI16 ? (data).ui16 :\
     ((data).storage == UI32 ? (data).ui32 :\
     (data).ui64))))))\
 )

#define TYPE_UNSIGNED (UI8 | UI16 | UI32 | UI64)
#define TYPE_SIGNED   ( I8 |  I16 |  I32 |  I64)

opcode_t ADD(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            /* If overflows */
            if (reg0->i8 + DATA_RETRIEVER(*reg1) > SCHAR_MAX)
                op_res.i8 = SCHAR_MIN + (reg0->i8 + DATA_RETRIEVER(*reg1) - SCHAR_MAX);
            /* If underflows */
            else if (reg0->i8 + DATA_RETRIEVER(*reg1) < SCHAR_MIN)
                op_res.i8 = SCHAR_MAX + (reg0->i8 + DATA_RETRIEVER(*reg1) - SCHAR_MIN);
            else
                op_res.i8 = reg0->i8 + DATA_RETRIEVER(*reg1);
            break;
        case UI8:
            op_res.ui8 = reg0->ui8 + DATA_RETRIEVER(*reg1);
            break;
        case I16:
            /* If overflows */
            if (reg0->i16 + DATA_RETRIEVER(*reg1) > SHRT_MAX)
                op_res.i16 = SHRT_MIN + (reg0->i16 + DATA_RETRIEVER(*reg1) - SHRT_MAX);
            /* If underflows */
            else if (reg0->i16 + DATA_RETRIEVER(*reg1) < SHRT_MIN)
                op_res.i16 = SHRT_MAX + (reg0->i16 + DATA_RETRIEVER(*reg1) - SHRT_MIN);
            else
                op_res.i16 = reg0->i16 + DATA_RETRIEVER(*reg1);
            break;
        case UI16:
            op_res.ui16 = reg0->ui16 + DATA_RETRIEVER(*reg1);
            break;
        case I32:
            /* If overflows */
            if (reg0->i32 + DATA_RETRIEVER(*reg1) > INT_MAX)
                op_res.i32 = INT_MIN + (reg0->i32 + DATA_RETRIEVER(*reg1) - INT_MAX);
            /* If underflows */
            else if (reg0->i32 + DATA_RETRIEVER(*reg1) < INT_MIN)
                op_res.i32 = INT_MAX + (reg0->i32 + DATA_RETRIEVER(*reg1) - INT_MIN);
            else
                op_res.i32 = reg0->i32 + DATA_RETRIEVER(*reg1);
            break;
        case UI32:
            op_res.ui32 = reg0->ui32 + DATA_RETRIEVER(*reg1);
            break;
        case I64:
            /* If overflows */
            if (reg0->i64 + DATA_RETRIEVER(*reg1) > LONG_MAX)
                op_res.i64 = LONG_MIN + (reg0->i64 + DATA_RETRIEVER(*reg1) - LONG_MAX);
            /* If underflows */
            else if (reg0->i64 + DATA_RETRIEVER(*reg1) < LONG_MIN)
                op_res.i64 = LONG_MAX + (reg0->i64 + DATA_RETRIEVER(*reg1) - LONG_MIN);
            else
                op_res.i64 = reg0->i64 + DATA_RETRIEVER(*reg1);
            break;
        case UI64:
            op_res.ui64 = reg0->ui64 + DATA_RETRIEVER(*reg1);
            break;
        case DBL:
            /* If overflows */
            if (reg0->dbl + DATA_RETRIEVER(*reg1) > DBL_MAX)
                op_res.dbl = DBL_MIN + (reg0->dbl + DATA_RETRIEVER(*reg1) - DBL_MAX);
            /* If underflows */
            else if (reg0->dbl + DATA_RETRIEVER(*reg1) < DBL_MIN)
                op_res.dbl = DBL_MAX + (reg0->dbl + DATA_RETRIEVER(*reg1) - DBL_MIN);
            else
                op_res.dbl = reg0->dbl + DATA_RETRIEVER(*reg1);
            break;
        case VA:
            op_res.va = reg0->va + DATA_RETRIEVER(*reg1);
            break;
    }
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t SUB(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            /* If overflows */
            if (reg0->i8 - DATA_RETRIEVER(*reg1) > SCHAR_MAX)
                op_res.i8 = SCHAR_MIN + (reg0->i8 - DATA_RETRIEVER(*reg1) - SCHAR_MAX);
            /* If underflows */
            else if (reg0->i8 - DATA_RETRIEVER(*reg1) < SCHAR_MIN)
                op_res.i8 = SCHAR_MAX + (reg0->i8 - DATA_RETRIEVER(*reg1) - SCHAR_MIN);
            else
                op_res.i8 = reg0->i8 - DATA_RETRIEVER(*reg1);
            break;
        case UI8:
            op_res.ui8 = reg0->ui8 - DATA_RETRIEVER(*reg1);
            break;
        case I16:
            /* If overflows */
            if (reg0->i16 - DATA_RETRIEVER(*reg1) > SHRT_MAX)
                op_res.i16 = SHRT_MIN + (reg0->i16 - DATA_RETRIEVER(*reg1) - SHRT_MAX);
            /* If underflows */
            else if (reg0->i16 - DATA_RETRIEVER(*reg1) < SHRT_MIN)
                op_res.i16 = SHRT_MAX + (reg0->i16 - DATA_RETRIEVER(*reg1) - SHRT_MIN);
            else
                op_res.i16 = reg0->i16 - DATA_RETRIEVER(*reg1);
            break;
        case UI16:
            op_res.ui16 = reg0->ui16 - DATA_RETRIEVER(*reg1);
            break;
        case I32:
            /* If overflows */
            if (reg0->i32 - DATA_RETRIEVER(*reg1) > INT_MAX)
                op_res.i32 = INT_MIN + (reg0->i32 - DATA_RETRIEVER(*reg1) - INT_MAX);
            /* If underflows */
            else if (reg0->i32 - DATA_RETRIEVER(*reg1) < INT_MIN)
                op_res.i32 = INT_MAX + (reg0->i32 - DATA_RETRIEVER(*reg1) - INT_MIN);
            else
                op_res.i32 = reg0->i32 - DATA_RETRIEVER(*reg1);
            break;
        case UI32:
            op_res.ui32 = reg0->ui32 - DATA_RETRIEVER(*reg1);
            break;
        case I64:
            /* If overflows */
            if (reg0->i64 - DATA_RETRIEVER(*reg1) > LONG_MAX)
                op_res.i64 = LONG_MIN + (reg0->i64 - DATA_RETRIEVER(*reg1) - LONG_MAX);
            /* If underflows */
            else if (reg0->i64 - DATA_RETRIEVER(*reg1) < LONG_MIN)
                op_res.i64 = LONG_MAX + (reg0->i64 - DATA_RETRIEVER(*reg1) - LONG_MIN);
            else
                op_res.i64 = reg0->i64 - DATA_RETRIEVER(*reg1);
            break;
        case UI64:
            op_res.ui64 = reg0->ui64 - DATA_RETRIEVER(*reg1);
            break;
        case DBL:
            /* If overflows */
            if (reg0->dbl - DATA_RETRIEVER(*reg1) > DBL_MAX)
                op_res.dbl = DBL_MIN + (reg0->dbl - DATA_RETRIEVER(*reg1) - DBL_MAX);
            /* If underflows */
            else if (reg0->dbl - DATA_RETRIEVER(*reg1) < DBL_MIN)
                op_res.dbl = DBL_MAX + (reg0->dbl - DATA_RETRIEVER(*reg1) - DBL_MIN);
            else
                op_res.dbl = reg0->dbl - DATA_RETRIEVER(*reg1);
            break;
        case VA:
            op_res.va = reg0->va - DATA_RETRIEVER(*reg1);
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t MUL(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            /* If overflows */
            if (reg0->i8 *DATA_RETRIEVER(*reg1) > SCHAR_MAX)
                op_res.i8 = SCHAR_MIN + (reg0->i8 *DATA_RETRIEVER(*reg1) - SCHAR_MAX);
            /* If underflows */
            else if (reg0->i8 *DATA_RETRIEVER(*reg1) < SCHAR_MIN)
                op_res.i8 = SCHAR_MAX + (reg0->i8 *DATA_RETRIEVER(*reg1) - SCHAR_MIN);
            else
                op_res.i8 = reg0->i8 *DATA_RETRIEVER(*reg1);
            break;
        case UI8:
            op_res.ui8 = reg0->ui8 *DATA_RETRIEVER(*reg1);
            break;
        case I16:
            /* If overflows */
            if (reg0->i16 *DATA_RETRIEVER(*reg1) > SHRT_MAX)
                op_res.i16 = SHRT_MIN + (reg0->i16 *DATA_RETRIEVER(*reg1) - SHRT_MAX);
            /* If underflows */
            else if (reg0->i16 *DATA_RETRIEVER(*reg1) < SHRT_MIN)
                op_res.i16 = SHRT_MAX + (reg0->i16 *DATA_RETRIEVER(*reg1) - SHRT_MIN);
            else
                op_res.i16 = reg0->i16 *DATA_RETRIEVER(*reg1);
            break;
        case UI16:
            op_res.ui16 = reg0->ui16 *DATA_RETRIEVER(*reg1);
            break;
        case I32:
            /* If overflows */
            if (reg0->i32 *DATA_RETRIEVER(*reg1) > INT_MAX)
                op_res.i32 = INT_MIN + (reg0->i32 *DATA_RETRIEVER(*reg1) - INT_MAX);
            /* If underflows */
            else if (reg0->i32 *DATA_RETRIEVER(*reg1) < INT_MIN)
                op_res.i32 = INT_MAX + (reg0->i32 *DATA_RETRIEVER(*reg1) - INT_MIN);
            else
                op_res.i32 = reg0->i32 *DATA_RETRIEVER(*reg1);
            break;
        case UI32:
            op_res.ui32 = reg0->ui32 *DATA_RETRIEVER(*reg1);
            break;
        case I64:
            /* If overflows */
            if (reg0->i64 *DATA_RETRIEVER(*reg1) > LONG_MAX)
                op_res.i64 = LONG_MIN + (reg0->i64 *DATA_RETRIEVER(*reg1) - LONG_MAX);
            /* If underflows */
            else if (reg0->i64 *DATA_RETRIEVER(*reg1) < LONG_MIN)
                op_res.i64 = LONG_MAX + (reg0->i64 *DATA_RETRIEVER(*reg1) - LONG_MIN);
            else
                op_res.i64 = reg0->i64 *DATA_RETRIEVER(*reg1);
            break;
        case UI64:
            op_res.ui64 = reg0->ui64 *DATA_RETRIEVER(*reg1);
            break;
        case DBL:
            /* If overflows */
            if (reg0->dbl *DATA_RETRIEVER(*reg1) > DBL_MAX)
                op_res.dbl = DBL_MIN + (reg0->dbl *DATA_RETRIEVER(*reg1) - DBL_MAX);
            /* If underflows */
            else if (reg0->dbl *DATA_RETRIEVER(*reg1) < DBL_MIN)
                op_res.dbl = DBL_MAX + (reg0->dbl *DATA_RETRIEVER(*reg1) - DBL_MIN);
            else
                op_res.dbl = reg0->dbl *DATA_RETRIEVER(*reg1);
            break;
        case VA:
            op_res.va = reg0->va *DATA_RETRIEVER(*reg1);
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t DIV(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            /* If overflows */
            if (reg0->i8 / DATA_RETRIEVER(*reg1) > SCHAR_MAX)
                op_res.i8 = SCHAR_MIN + (reg0->i8 / DATA_RETRIEVER(*reg1) - SCHAR_MAX);
            /* If underflows */
            else if (reg0->i8 / DATA_RETRIEVER(*reg1) < SCHAR_MIN)
                op_res.i8 = SCHAR_MAX + (reg0->i8 / DATA_RETRIEVER(*reg1) - SCHAR_MIN);
            else
                op_res.i8 = reg0->i8 / DATA_RETRIEVER(*reg1);
            break;
        case UI8:
            op_res.ui8 = reg0->ui8 / DATA_RETRIEVER(*reg1);
            break;
        case I16:
            /* If overflows */
            if (reg0->i16 / DATA_RETRIEVER(*reg1) > SHRT_MAX)
                op_res.i16 = SHRT_MIN + (reg0->i16 / DATA_RETRIEVER(*reg1) - SHRT_MAX);
            /* If underflows */
            else if (reg0->i16 / DATA_RETRIEVER(*reg1) < SHRT_MIN)
                op_res.i16 = SHRT_MAX + (reg0->i16 / DATA_RETRIEVER(*reg1) - SHRT_MIN);
            else
                op_res.i16 = reg0->i16 / DATA_RETRIEVER(*reg1);
            break;
        case UI16:
            op_res.ui16 = reg0->ui16 / DATA_RETRIEVER(*reg1);
            break;
        case I32:
            /* If overflows */
            if (reg0->i32 / DATA_RETRIEVER(*reg1) > INT_MAX)
                op_res.i32 = INT_MIN + (reg0->i32 / DATA_RETRIEVER(*reg1) - INT_MAX);
            /* If underflows */
            else if (reg0->i32 / DATA_RETRIEVER(*reg1) < INT_MIN)
                op_res.i32 = INT_MAX + (reg0->i32 / DATA_RETRIEVER(*reg1) - INT_MIN);
            else
                op_res.i32 = reg0->i32 / DATA_RETRIEVER(*reg1);
            break;
        case UI32:
            op_res.ui32 = reg0->ui32 / DATA_RETRIEVER(*reg1);
            break;
        case I64:
            /* If overflows */
            if (reg0->i64 / DATA_RETRIEVER(*reg1) > LONG_MAX)
                op_res.i64 = LONG_MIN + (reg0->i64 / DATA_RETRIEVER(*reg1) - LONG_MAX);
            /* If underflows */
            else if (reg0->i64 / DATA_RETRIEVER(*reg1) < LONG_MIN)
                op_res.i64 = LONG_MAX + (reg0->i64 / DATA_RETRIEVER(*reg1) - LONG_MIN);
            else
                op_res.i64 = reg0->i64 / DATA_RETRIEVER(*reg1);
            break;
        case UI64:
            op_res.ui64 = reg0->ui64 / DATA_RETRIEVER(*reg1);
            break;
        case DBL:
            /* If overflows */
            if (reg0->dbl / DATA_RETRIEVER(*reg1) > DBL_MAX)
                op_res.dbl = DBL_MIN + (reg0->dbl / DATA_RETRIEVER(*reg1) - DBL_MAX);
            /* If underflows */
            else if (reg0->dbl / DATA_RETRIEVER(*reg1) < DBL_MIN)
                op_res.dbl = DBL_MAX + (reg0->dbl / DATA_RETRIEVER(*reg1) - DBL_MIN);
            else
                op_res.dbl = reg0->dbl / DATA_RETRIEVER(*reg1);
            break;
        case VA:
            op_res.va = reg0->va / DATA_RETRIEVER(*reg1);
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t MOD(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            /* If overflows */
            if (reg0->i8 % DATA_RETRIEVER_MATHS(*reg1) > SCHAR_MAX)
                op_res.i8 = SCHAR_MIN + (reg0->i8 % DATA_RETRIEVER_MATHS(*reg1) - SCHAR_MAX);
            /* If underflows */
            else if (reg0->i8 % DATA_RETRIEVER_MATHS(*reg1) < SCHAR_MIN)
                op_res.i8 = SCHAR_MAX + (reg0->i8 % DATA_RETRIEVER_MATHS(*reg1) - SCHAR_MIN);
            else
                op_res.i8 = reg0->i8 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = reg0->ui8 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            /* If overflows */
            if (reg0->i16 % DATA_RETRIEVER_MATHS(*reg1) > SHRT_MAX)
                op_res.i16 = SHRT_MIN + (reg0->i16 % DATA_RETRIEVER_MATHS(*reg1) - SHRT_MAX);
            /* If underflows */
            else if (reg0->i16 % DATA_RETRIEVER_MATHS(*reg1) < SHRT_MIN)
                op_res.i16 = SHRT_MAX + (reg0->i16 % DATA_RETRIEVER_MATHS(*reg1) - SHRT_MIN);
            else
                op_res.i16 = reg0->i16 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
                op_res.ui16 = reg0->ui16 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            /* If overflows */
            if (reg0->i32 % DATA_RETRIEVER_MATHS(*reg1) > INT_MAX)
                op_res.i32 = INT_MIN + (reg0->i32 % DATA_RETRIEVER_MATHS(*reg1) - INT_MAX);
            /* If underflows */
            else if (reg0->i32 % DATA_RETRIEVER_MATHS(*reg1) < INT_MIN)
                op_res.i32 = INT_MAX + (reg0->i32 % DATA_RETRIEVER_MATHS(*reg1) - INT_MIN);
            else
                op_res.i32 = reg0->i32 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = reg0->ui32 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            /* If overflows */
            if (reg0->i64 % DATA_RETRIEVER_MATHS(*reg1) > LONG_MAX)
                op_res.i64 = LONG_MIN + (reg0->i64 % DATA_RETRIEVER_MATHS(*reg1) - LONG_MAX);
            /* If underflows */
            else if (reg0->i64 % DATA_RETRIEVER_MATHS(*reg1) < LONG_MIN)
                op_res.i64 = LONG_MAX + (reg0->i64 % DATA_RETRIEVER_MATHS(*reg1) - LONG_MIN);
            else
                op_res.i64 = reg0->i64 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = reg0->ui64 % DATA_RETRIEVER_MATHS(*reg1);
            break;
        case DBL:
            break;
        case VA:
            op_res.va = reg0->va % DATA_RETRIEVER_MATHS(*reg1);
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t AND(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            op_res.i16 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
            op_res.ui16 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            op_res.i32 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            op_res.i64 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        case VA:
            op_res.va = DATA_RETRIEVER_MATHS(*reg0) & DATA_RETRIEVER_MATHS(*reg1);
            break;
        default:
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t OR(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            op_res.i16 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
            op_res.ui16 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            op_res.i32 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            op_res.i64 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        case VA:
            op_res.va = DATA_RETRIEVER_MATHS(*reg0) | DATA_RETRIEVER_MATHS(*reg1);
            break;
        default:
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t XOR(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            op_res.i16 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
            op_res.ui16 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            op_res.i32 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            op_res.i64 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        case VA:
            op_res.va = DATA_RETRIEVER_MATHS(*reg0) ^ DATA_RETRIEVER_MATHS(*reg1);
            break;
        default:
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t NOT(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;

    /* NOT */
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = ~DATA_RETRIEVER_MATHS(*reg0);
        case I16:
            op_res.i16 = ~DATA_RETRIEVER_MATHS(*reg0);
        case I32:
            op_res.i32 = ~DATA_RETRIEVER_MATHS(*reg0);
        case I64:
            op_res.i64 = ~DATA_RETRIEVER_MATHS(*reg0);
        case UI8:
            op_res.ui8 = ~DATA_RETRIEVER_MATHS(*reg0);
        case UI16:
            op_res.ui16 = ~DATA_RETRIEVER_MATHS(*reg0);
        case UI32:
            op_res.ui32 = ~DATA_RETRIEVER_MATHS(*reg0);
        case UI64:
            op_res.ui64 = ~DATA_RETRIEVER_MATHS(*reg0);
        case DBL:
            op_res.dbl = ~DATA_RETRIEVER_MATHS(*reg0);
        case VA:
            op_res.va = ~DATA_RETRIEVER_MATHS(*reg0);
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t LSHIFT(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];

    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            op_res.i16 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
            op_res.ui16 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            op_res.i32 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            op_res.i64 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        case VA:
            op_res.va = DATA_RETRIEVER_MATHS(*reg0) << DATA_RETRIEVER_MATHS(*reg1);
            break;
        default:
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t RSHIFT(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];

    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = reg0->storage;
    switch (op_res.storage)
    {
        case I8:
            op_res.i8 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI8:
            op_res.ui8 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I16:
            op_res.i16 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI16:
            op_res.ui16 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I32:
            op_res.i32 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI32:
            op_res.ui32 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case I64:
            op_res.i64 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case UI64:
            op_res.ui64 = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        case VA:
            op_res.va = DATA_RETRIEVER_MATHS(*reg0) >> DATA_RETRIEVER_MATHS(*reg1);
            break;
        default:
            break;
    }

    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

/* END ARITHMETIC & BITWISE OPERATIONS */

/*
 *RELATIONAL & LOGICAL OPERATIONS
 */

opcode_t LESS(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) < DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t LESS_EQ(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) <= DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t GREAT(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) > DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t GREAT_EQ(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) >= DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t EQUAL(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) == DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t N_EQUAL(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) != DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t LOG_AND(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) && DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t LOG_OR(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} {2:REGISTER_ADDRESS_1} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, *reg1, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    /* Fetch REGISTER_ADDRESS_1 */
    reg1 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = DATA_RETRIEVER(*reg0) || DATA_RETRIEVER(*reg1) ? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

opcode_t LOG_NOT(VM *vm, va_t tid) /* {0:OPCODE} {1:REGISTER_ADDRESS_0} */
{
    Thread *thread = &vm->core.thread_pool[tid];
    PrimitiveData *reg0, op_res;

    /* Fetch REGISTER_ADDRESS_0 */
    reg0 = fetch_reg(vm, tid);

    op_res.storage = I8;
    op_res.i8 = !DATA_RETRIEVER(*reg0)? 1 : 0;
    thread->controlunit.aritreg = op_res;

    return thread->controlunit.instrreg;
}

/* END RELATIONAL & LOGICAL OPERATIONS */

/*
 *UTILITY FUNCTIONS
 */

PrimitiveData *fetch_reg(VM *vm, va_t tid)
{
    opcode_t regid = fetch_code(vm, tid);
    PrimitiveData *reg;

    switch (regid)
    {
        case 0x00:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[0];
            break;
        case 0x01:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[1];
            break;
        case 0x02:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[2];
            break;
        case 0x04:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[3];
            break;
        case 0x08:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[4];
            break;
        case 0x10:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[5];
            break;
        case 0x20:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[6];
            break;
        case 0x40:
            reg = &vm->core.thread_pool[tid].controlunit.genpreg[7];
            break;
        case 0x80:
            reg = &vm->core.thread_pool[tid].controlunit.aritreg;
            break;
    }

    return reg;
}

inline uint8_t fetch_code(VM *vm, va_t tid)
{
    return vm->codeseg.content[vm->core.thread_pool[tid].controlunit.instrpointreg++];
}

/* END UTILITY FUNCTIONS */
