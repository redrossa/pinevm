#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t  opcode_t;
typedef uint64_t va_t;

typedef struct PineVM VM;

typedef struct PineVMData
{
    enum
    {
        I8  = 0x01, UI8  = 0x02,
        I16 = 0x04, UI16 = 0x08,
        I32 = 0x10, UI32 = 0x20,
        I64 = 0x40, UI64 = 0x80,
        DBL = 0x100,
        VA  = 0x200
    } storage;
    union
    {
        int8_t      i8;
        uint8_t     ui8;
        int16_t     i16;
        uint16_t    ui16;
        int32_t     i32;
        uint32_t    ui32;
        int64_t     i64;
        uint64_t    ui64;
        double      dbl;
        va_t        va; /* uint64_t */
    };
} PrimitiveData;

extern int pvm_Handle(int, const char *, int);
extern int pvm_ReportError(int, const char *, const char *);

#define VA_GETSIZE(vadr) ((vadr) + 1)
#define VA_GETVADR(size) ((size) - 1)
#define VA_SUCCEED(vadr) ((vadr) + 1)
#define VA_PRECEED(vadr) ((vadr) - 1)

#endif /* COMMON_H */
