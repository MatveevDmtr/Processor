#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include "StackConfig.h"

#define StructStackInit(name)                              \
        StructureStackInit(#name,                          \
                           __PRETTY_FUNCTION__,            \
                           __FILE__,                       \
                           __LINE__);

#if STACK_MODE >= HARDDEBUG_MODE
    #define StackDump(stk);                                    \
            FuckingDump(stk,                                   \
                        __PRETTY_FUNCTION__,                   \
                        __FILE__,                              \
                        __LINE__);
#else if
    #define StackDump(stk);
#endif

typedef unsigned long long UnsignedLL;

typedef unsigned long long bird_t;

const bird_t LEFT_CANNARY  = 0xDEDFED15;

const bird_t RIGHT_CANNARY = 0xCA115FA1;

const UnsignedLL START_HASH = 88005553535;

typedef struct stack_info
{
    bird_t             StructLeftCannary;

    elem_t*            Ptr;
    size_t             Size;
    size_t             Capacity;

    bool               DeadInside;
    bird_t*            PtrStackLeftBird;
    bird_t*            PtrStackRightBird;

#if STACK_MODE >= HASH_MODE
    UnsignedLL         StackHashSum;
    UnsignedLL         StructHashSum;
#endif

#if STACK_MODE >= HARDDEBUG_MODE
    struct  calling_inf
    {
        const char* Orig_name;
        const char* Func_calling;
        const char* File_calling;
        size_t      Line_created;
    } debug_info;
#endif

    bird_t StructRightCannary;
}
stack_t;

stack_t StructureStackInit(const char* name,
                           const char* func,
                           const char* file,
                           int line);

UnsignedLL FindErrors(stack_t* stk);

int DecodeErrors(UnsignedLL sum_errcodes);

int LogCritError(int errcode, const char* func, int line);

UnsignedLL StackVerify(stack_t* stk);

#if STACK_MODE >= HARDDEBUG_MODE
void DumpEmExit();

int FuckingDump(stack_t* stk,
                const char* funcname,
                const char* filename,
                int line);
#endif

int StackCtor(stack_t* stk);

int StackDtor(stack_t* stk);

static int StackResize(stack_t* stk, size_t new_capacity);

int StackPush(stack_t* stk, elem_t elem);

elem_t StackPop(stack_t* stk);

#if STACK_MODE >= HASH_MODE
UnsignedLL CalculateGNUHash(void* start_ptr, size_t num_bytes);

static int UpdateHash(stack_t* stk);
#endif

int CheckLeftCannary  (stack_t* stk);

int CheckRightCannary (stack_t* stk);


#endif // guard

